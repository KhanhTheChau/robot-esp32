#include "AudioPlayer.h"
#include "../config/AppConfig.h"

AudioPlayer::AudioPlayer(ILogger& logger)
    : logger(logger)
{
}

AudioPlayer::~AudioPlayer()
{
    i2s_driver_uninstall(I2S_PORT);
}

void AudioPlayer::begin()
{
    logger.info("AudioPlayer::begin()");
    initI2S();
}

void AudioPlayer::initI2S()
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = AppConfig::I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // Bắt buộc phải là Stereo để MAX98357 nhận đủ xung nhịp (32 BCLK/LRCK)
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = AppConfig::I2S_SPK_BCLK_PIN,
        .ws_io_num = AppConfig::I2S_SPK_LRC_PIN,
        .data_out_num = AppConfig::I2S_SPK_DIN_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0))
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, nullptr);
    if (err != ESP_OK)
    {
        logger.error("Failed installing I2S driver for AudioPlayer");
    }

    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK)
    {
        logger.error("Failed setting I2S pins for AudioPlayer");
    }
}

void AudioPlayer::write(const uint8_t* data, size_t size)
{
    // Dữ liệu tải về từ Python Server là Raw PCM (16-bit Mono)
    // MAX98357 yêu cầu luồng I2S Stereo chuẩn. Do đó ta cần nhân đôi mẫu (Mono -> Stereo)
    size_t numSamples = size / 2;
    int16_t* monoData = (int16_t*)data;
    
    // Buffer tạm chứa dữ liệu Stereo
    int16_t stereoBuffer[128]; // 64 mẫu Mono -> 64 mẫu Left + 64 mẫu Right (tổng 128 int16_t = 256 bytes)
    size_t sampleIndex = 0;
    
    while (sampleIndex < numSamples)
    {
        size_t samplesRemaining = numSamples - sampleIndex;
        size_t samplesToProcess = (samplesRemaining < 64) ? samplesRemaining : 64;
        
        for (size_t i = 0; i < samplesToProcess; i++)
        {
            int32_t sample = monoData[sampleIndex + i];
            sample = sample * 4; // Kích âm lượng x4 để đảm bảo không bị quá bé
            
            // Cắt gọn nếu vượt ngưỡng 16-bit
            if (sample > 32767) sample = 32767;
            if (sample < -32768) sample = -32768;

            stereoBuffer[i*2] = (int16_t)sample;     // Kênh Trái
            stereoBuffer[i*2 + 1] = (int16_t)sample; // Kênh Phải
        }
        
        size_t bytesToWrite = samplesToProcess * 4; // 2 bytes * 2 kênh
        size_t bytesWritten = 0;
        i2s_write(I2S_PORT, stereoBuffer, bytesToWrite, &bytesWritten, portMAX_DELAY);
        
        sampleIndex += samplesToProcess;
    }
}
