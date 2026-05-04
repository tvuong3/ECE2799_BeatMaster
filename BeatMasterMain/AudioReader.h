#ifndef AUDIOREADER_H
#define AUDIOREADER_H

#include "ESP_I2S.h"
#include "FS.h"
#include "SDReader.h"

#define I2S_BCLK      17
#define I2S_LRC       21
#define I2S_DOUT      22
#define BUFFER_SIZE   4096

extern I2SClass i2s;
extern float globalVolume;

// Structure to hold WAV header info
struct WavHeader {
    char riff[4];           // "RIFF"
    uint32_t fileSize;
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmtLen;
    uint16_t formatTag;     // 1 = PCM
    uint16_t channels;      // 1 = Mono, 2 = Stereo
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char data[4];           // "data"
    uint32_t dataSize;
};

struct AudioRequest {
    char path[64];
    float volume;
};

bool setup_i2s_for_file(uint32_t rate, uint16_t bits, uint16_t channels);
void audioManagerTask(void* pvParameters);
void play_wav_from_sd(const char* path, float volume);

#endif