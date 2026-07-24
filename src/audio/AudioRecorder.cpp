#include "AudioRecorder.h"
#include "../config/AppConfig.h"

AudioRecorder::AudioRecorder(ILogger& logger)
    : logger(logger), recordedSize(0), isRecording(false)
{
}

AudioRecorder::~AudioRecorder()
{
    if (isRecording)
    {
        stopRecording();
    }
    i2s_driver_uninstall(I2S_PORT);
}

void AudioRecorder::begin()
{
    logger.info("AudioRecorder::begin()");
    audioBuffer.reserve(MAX_AUDIO_BUFFER_SIZE);
    initI2S();
}

void AudioRecorder::initI2S()
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AppConfig::I2S_SAMPLE_RATE,
        // Dùng 16-bit trực tiếp như DroneBotWorkshop (Phần cứng I2S tự xử lý việc cắt 24-bit của INMP441)
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = AppConfig::I2S_SCK_PIN,
        .ws_io_num = AppConfig::I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = AppConfig::I2S_SD_PIN
    };
    
    // RẤT QUAN TRỌNG CHO ESP32-S3: Giữ nguyên dòng này để không phá hỏng nút BOOT
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0))
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, nullptr);
    if (err != ESP_OK)
    {
        logger.error("Failed installing I2S driver");
    }

    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK)
    {
        logger.error("Failed setting I2S pins");
    }
}

void AudioRecorder::startRecording()
{
    if (isRecording) return;
    
    logger.info("Starting recording...");
    audioBuffer.clear();
    recordedSize = 0;
    isRecording = true;
    
    xTaskCreate(i2sTaskHelper, "I2SRecordTask", 8192, this, 1, nullptr);
}

void AudioRecorder::stopRecording()
{
    if (!isRecording) return;
    
    logger.info("Stopping recording...");
    isRecording = false;
    delay(20);
}

const uint8_t* AudioRecorder::getAudioBuffer() const
{
    return audioBuffer.data();
}

size_t AudioRecorder::getAudioBufferSize() const
{
    return recordedSize;
}

void AudioRecorder::i2sTaskHelper(void* pvParameters)
{
    AudioRecorder* recorder = static_cast<AudioRecorder*>(pvParameters);
    if (recorder)
    {
        recorder->recordTask();
    }
    vTaskDelete(nullptr);
}

void AudioRecorder::recordTask()
{
    size_t bytesRead = 0;
    int16_t tempBuffer16[512]; // Đọc trực tiếp ra 16-bit

    while (isRecording && recordedSize < MAX_AUDIO_BUFFER_SIZE)
    {
        esp_err_t result = i2s_read(I2S_PORT, &tempBuffer16, sizeof(tempBuffer16), &bytesRead, portMAX_DELAY);
        
        if (result == ESP_OK && bytesRead > 0)
        {
            size_t bytesToAppend = bytesRead;
            
            if (recordedSize + bytesToAppend > MAX_AUDIO_BUFFER_SIZE)
            {
                bytesToAppend = MAX_AUDIO_BUFFER_SIZE - recordedSize;
            }
            
            uint8_t* bytePtr = reinterpret_cast<uint8_t*>(tempBuffer16);
            audioBuffer.insert(audioBuffer.end(), bytePtr, bytePtr + bytesToAppend);
            recordedSize += bytesToAppend;
        }
        else 
        {
            delay(10);
        }
    }
    
    isRecording = false;
    logger.info("Recording task finished");
}

