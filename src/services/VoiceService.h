#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#include "../core/ILogger.h"
#include "../audio/IAudioRecorder.h"
#include "../network/IHttpClient.h"
#include "../models/VoiceResult.h"

class VoiceService
{
public:
    VoiceService(ILogger& logger, IAudioRecorder& recorder, IHttpClient& httpClient);

    void begin();
    
    void startRecording();
    VoiceResult stopAndUpload();

private:
    ILogger& logger;
    IAudioRecorder& recorder;
    IHttpClient& httpClient;
};

#endif // VOICE_SERVICE_H
