#ifndef IHTTP_CLIENT_H
#define IHTTP_CLIENT_H

#include "../models/VoiceResult.h"

enum class UploadResult
{
    Success,
    Timeout,
    ConnectionFailed,
    ServerError,
    InvalidResponse
};

class IHttpClient
{
public:
    virtual ~IHttpClient() = default;

    virtual UploadResult sendAudio(const uint8_t* audioData, size_t dataSize, VoiceResult& outResult) = 0;
};

#endif // IHTTP_CLIENT_H
