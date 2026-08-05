#include "AudioRecorder.h"
#include "../config/AppConfig.h"
#include <math.h>

AudioRecorder::AudioRecorder(ILogger& logger)
    : logger(logger)
{
}

AudioRecorder::~AudioRecorder()
{
    i2s_driver_uninstall(I2S_PORT);
}

void AudioRecorder::begin()
{
    logger.info("AudioRecorder::begin()");
    initI2S();
}

void AudioRecorder::initI2S()
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AppConfig::I2S_SAMPLE_RATE,
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

size_t AudioRecorder::readAudioChunk(uint8_t* buffer, size_t maxLen)
{
    if (_isPaused) {
        return 0; // Không đọc dữ liệu khi đang bị pause (ví dụ đang phát TTS)
    }

    size_t bytesRead = 0;
    // Dùng timeout 50ms để đợi đủ 1024 bytes (chiếm 32ms ở 16kHz)
    // Điều này sẽ không block hoàn toàn mạch nhưng đảm bảo lấy đủ mẫu để tính RMS chính xác.
    i2s_read(I2S_PORT, buffer, maxLen, &bytesRead, 50 / portTICK_PERIOD_MS);
    return bytesRead;
}

void AudioRecorder::pause()
{
    _isPaused = true;
    // Có thể dọn dẹp I2S queue nếu cần thiết, ví dụ: i2s_zero_dma_buffer(I2S_PORT);
}

void AudioRecorder::resume()
{
    _isPaused = false;
    i2s_zero_dma_buffer(I2S_PORT); // Xóa buffer rác trong I2S DMA
}

float AudioRecorder::calculateRMS(const uint8_t* buffer, size_t len)
{
    if (len == 0) return 0.0f;
    
    int16_t* samples = (int16_t*)buffer;
    size_t numSamples = len / 2;
    
    float sumSquares = 0.0f;
    for (size_t i = 0; i < numSamples; i++)
    {
        float val = (float)samples[i];
        sumSquares += (val * val);
    }
    
    return sqrt(sumSquares / numSamples);
}
