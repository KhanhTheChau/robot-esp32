#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#include "../core/ILogger.h"
#include "../audio/IAudioRecorder.h"
#include "../audio/IAudioPlayer.h"
#include "../network/IHttpClient.h"
#include "../models/VoiceResult.h"

class VoiceService
{
public:
    VoiceService(ILogger& logger, IAudioRecorder& recorder, IAudioPlayer& player, IHttpClient& httpClient);

    void begin();
    
    void startRecording();
    VoiceResult stopAndUpload();
    int playResponse(const String& url);

private:
    ILogger& logger;
    IAudioRecorder& recorder;
    IAudioPlayer& player;
    IHttpClient& httpClient;
};

#endif // VOICE_SERVICE_H
