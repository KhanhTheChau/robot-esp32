#ifndef VOICE_RESULT_H
#define VOICE_RESULT_H

#include <Arduino.h>

class VoiceResult
{
public:
    bool success;
    String text;
    String intent;
    float confidence;
    String audioUrl;

    VoiceResult() : success(false), confidence(0.0f) {}
};

#endif // VOICE_RESULT_H
