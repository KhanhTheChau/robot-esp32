#ifndef VOICE_RESULT_H
#define VOICE_RESULT_H

#include <Arduino.h>

class VoiceResult
{
public:
    bool success;
    String action;
    String text;
    String intent;
    float confidence;
    String audioUrl;
    String emotion;

    VoiceResult() : success(false), confidence(0.0f), emotion("neutral") {}
};

#endif // VOICE_RESULT_H
