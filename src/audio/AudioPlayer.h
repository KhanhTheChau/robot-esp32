#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include "IAudioPlayer.h"
#include "../core/ILogger.h"
#include <driver/i2s.h>

class AudioPlayer : public IAudioPlayer
{
public:
    explicit AudioPlayer(ILogger& logger);
    ~AudioPlayer() override;

    void begin() override;
    void write(const uint8_t* data, size_t size) override;

private:
    ILogger& logger;
    const i2s_port_t I2S_PORT = I2S_NUM_1; // Kênh I2S số 1 dành cho Audio Output
    void initI2S();
};

#endif // AUDIO_PLAYER_H
