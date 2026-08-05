import asyncio
import json
import logging
import os
import websockets
from core.stt_engine import STTEngine
from core.llm_engine import LLMEngine
from core.tts_engine import TTSEngine
from core.phrase_manager import PhraseManager

class ClientSession:
    def __init__(self, websocket, stt: STTEngine, llm: LLMEngine, tts: TTSEngine, phrase: PhraseManager):
        self.websocket = websocket
        self.stt = stt
        self.llm = llm
        self.tts = tts
        self.phrase = phrase
        
        self.audio_buffer = bytearray()
        self.state = "SLEEP"  # SLEEP or AWAKE
        
    async def start(self):
        try:
            async for message in self.websocket:
                if isinstance(message, bytes):
                    self.audio_buffer.extend(message)
                else:
                    try:
                        data = json.loads(message)
                        if data.get("action") == "end_of_speech":
                            logging.info(f"Received end_of_speech. Audio size: {len(self.audio_buffer)} bytes. State: {self.state}")
                            if len(self.audio_buffer) > 0:
                                # Start a background task so websocket can continue receiving if needed
                                # However, robot pauses VAD while waiting/speaking, so we can await directly.
                                await self.process_audio()
                            self.audio_buffer.clear()
                    except json.JSONDecodeError:
                        logging.error("Invalid JSON from ESP32")
        except websockets.exceptions.ConnectionClosed as e:
            logging.info(f"WebSocket closed: {e}")
            
    async def stream_audio_file(self, file_path: str, wav_path: str = None):
        """Stream a PCM file to ESP32 and play WAV on PC."""
        if wav_path and os.path.exists(wav_path):
            try:
                import winsound
                logging.info(f"Đang phát loa PC (demo): {wav_path}")
                winsound.PlaySound(wav_path, winsound.SND_FILENAME | winsound.SND_ASYNC)
            except ImportError:
                pass

        if not os.path.exists(file_path):
            logging.error(f"Audio file not found: {file_path}")
            return
            
        logging.info(f"Streaming audio to ESP32: {file_path}")
        with open(file_path, "rb") as f:
            while True:
                chunk = f.read(1024)
                if not chunk:
                    break
                await self.websocket.send(chunk)
                await asyncio.sleep(0.01) # Prevent network buffer overflow

    async def process_audio(self):
        pcm_data = bytes(self.audio_buffer)
        
        if self.state == "SLEEP":
            text = await self.stt.recognize_audio(pcm_data)
            if not text:
                await self.websocket.send(json.dumps({"action": "ERROR"}))
                return
                
            if self.stt.contains_wake_word(text):
                logging.info("Wake word detected! Switching to AWAKE.")
                self.state = "AWAKE"
                
                phrase_text, audio_path, wav_path = self.phrase.get_random_phrase_audio("wake")
                await self.websocket.send(json.dumps({
                    "action": "WAKE_UP",
                    "text": phrase_text
                }))
                await self.stream_audio_file(audio_path, wav_path)
            else:
                logging.info("No wake word. Ignoring.")
                # Gửi ERROR để giải phóng ESP32 khỏi trạng thái PROCESSING
                await self.websocket.send(json.dumps({"action": "ERROR"}))
                
        elif self.state == "AWAKE":
            # 1. Phát câu "đang suy nghĩ" ngay lập tức để robot phản hồi nhanh
            phrase_text, audio_path, wav_path = self.phrase.get_random_phrase_audio("thinking")
            await self.websocket.send(json.dumps({
                "action": "THINKING",
                "text": phrase_text
            }))
            await self.stream_audio_file(audio_path, wav_path)
            
            # 2. Chuyển giọng nói thành văn bản
            text = await self.stt.recognize_audio(pcm_data)
            if not text:
                # Nếu không nghe rõ, gửi ERROR để ESP32 về lại IDLE
                await self.websocket.send(json.dumps({"action": "ERROR"}))
                return
                
            if self.stt.contains_sleep_word(text):
                logging.info("Sleep word detected locally! Bypassing LLM. Switching to SLEEP.")
                self.state = "SLEEP"
                gb_text, gb_audio, gb_wav = self.phrase.get_random_phrase_audio("goodbye")
                await self.websocket.send(json.dumps({
                    "action": "GO_TO_SLEEP",
                    "text": gb_text
                }))
                await self.stream_audio_file(gb_audio, gb_wav)
                return

            # 3. Phân tích ngữ nghĩa qua LLM
            ai_response, ai_emotion = await self.llm.generate_response(text)
            
            # Dự phòng LLM vẫn có thể trả về goodbye nếu câu nói lắt léo
            if ai_emotion.lower() == "goodbye":
                logging.info("Goodbye detected via LLM. Switching to SLEEP.")
                self.state = "SLEEP"
                gb_text, gb_audio, gb_wav = self.phrase.get_random_phrase_audio("goodbye")
                await self.websocket.send(json.dumps({
                    "action": "GO_TO_SLEEP",
                    "text": gb_text
                }))
                await self.stream_audio_file(gb_audio, gb_wav)
            else:
                # 4. Gửi kết quả Chat và Stream Audio
                tts_pcm_path, tts_wav_path = await self.tts.generate_pcm(ai_response)
                
                await self.websocket.send(json.dumps({
                    "action": "CHAT_RESPONSE",
                    "text": ai_response,
                    "emotion": ai_emotion
                }))
                
                if tts_pcm_path:
                    await self.stream_audio_file(tts_pcm_path, tts_wav_path)
                    self.tts.cleanup_file(tts_pcm_path)
                    self.tts.cleanup_file(tts_wav_path)
