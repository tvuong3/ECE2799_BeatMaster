#include "AudioReader.h"

#define VOLUME_CONTROL_PIN 39

I2SClass i2s;
float globalVolume = 0.2;
extern QueueHandle_t audioQueue;

/**
 * Dynamically configures I2S based on file properties
 */
bool setup_i2s_for_file(uint32_t rate, uint16_t bits, uint16_t channels) {
    i2s.end();
    delay(50);
    i2s.setPins(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
    i2s_slot_mode_t slot_mode = (channels == 1) ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO;
    if(slot_mode == I2S_SLOT_MODE_STEREO) {
        Serial.println("Yeah this shit stereo");
    }
    
    // bits == 16 typically, but we cast to the driver's enum
    i2s_data_bit_width_t bit_width;
    if (bits == 8) {
        bit_width = I2S_DATA_BIT_WIDTH_8BIT;
    } else if (bits == 24) {
        // Set bit width to 32-bit
        // Must perform bitshifting on the data so that it can be read as 32-bit audio
        bit_width = I2S_DATA_BIT_WIDTH_32BIT;
    } else if (bits == 32) {
        bit_width = I2S_DATA_BIT_WIDTH_32BIT;
    } else {
        bit_width = I2S_DATA_BIT_WIDTH_16BIT;
    }
    
    if (!i2s.begin(I2S_MODE_STD, rate, bit_width, slot_mode, I2S_STD_SLOT_BOTH)) {
        Serial.println("I2S Config Failed!");
        return false;
    }
    return true;
}

void audioManagerTask(void* pvParameters) {
    AudioRequest request;
    bool hasRequest = false;
    
    uint8_t* rawBuffer = (uint8_t*)malloc(BUFFER_SIZE);
    int32_t* processedBuffer = (int32_t*)malloc(BUFFER_SIZE * sizeof(int32_t));

    while (true) {
        // If we don't have an active request, wait indefinitely for one
        if (!hasRequest) {
            xQueueReceive(audioQueue, &request, portMAX_DELAY);
            hasRequest = true;
        }

        // --- Start Playback Process ---
        File file = SD.open(request.path);
        if (!file) {
            hasRequest = false;
            continue;
        }

        WavHeader header;
        if (file.read((uint8_t*)&header, 44) == 44) {
            i2s_data_bit_width_t i2sWidth = (header.bitsPerSample > 16) ? I2S_DATA_BIT_WIDTH_32BIT : I2S_DATA_BIT_WIDTH_16BIT;
            setup_i2s_for_file(header.sampleRate, i2sWidth, header.channels);

            while (file.available()) {
                // IMPORTANT: Check if a NEW drum hit has been requested
                AudioRequest nextRequest;
                if (xQueueReceive(audioQueue, &nextRequest, 0) == pdTRUE) {
                    // Switch the current request to the new one and break the inner loop
                    request = nextRequest;
                    break; // This stops the current audio file immediately
                }

                int bytesRead = file.read(rawBuffer, BUFFER_SIZE);
                int bytesPerSample = header.bitsPerSample / 8;
                int samplesCount = bytesRead / bytesPerSample;
                size_t bytesToWrite = 0;

                for (int i = 0; i < samplesCount; i++) {
                    int32_t sample = 0;
                    uint8_t* p = &rawBuffer[i * bytesPerSample];
                    
                    if (header.bitsPerSample == 16) sample = (int32_t)((int16_t)(p[1] << 8 | p[0]));
                    else if (header.bitsPerSample == 24) {
                        sample = (int32_t)((p[2] << 24) | (p[1] << 16) | (p[0] << 8));
                        if (sample & 0x800000) sample |= 0xFF000000;
                    }
                    else if (header.bitsPerSample == 32) sample = (int32_t)((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);

                    sample = (int32_t)(sample * request.volume);

                    if (i2sWidth == I2S_DATA_BIT_WIDTH_16BIT) {
                        ((int16_t*)processedBuffer)[i] = (int16_t)sample;
                        bytesToWrite = samplesCount * 2;
                    } else {
                        processedBuffer[i] = sample;
                        bytesToWrite = samplesCount * 4;
                    }
                }
                i2s.write((uint8_t*)processedBuffer, bytesToWrite);

                // If we reach the end of the file naturally
                if (!file.available()) {
                    hasRequest = false;
                }
            }
        } else {
            hasRequest = false; 
        }
        file.close();
        // The loop will now either restart with the 'new' request 
        // or go back to the top to wait for a request.
    }
}

void play_wav_from_sd(const char* path, float volume) {
    AudioRequest req;
    strncpy(req.path, path, sizeof(req.path));
    req.volume = (volume > 1.0f) ? 1.0f : volume;

    // Send the hit to the queue. 
    // We use a 0 timeout so if the queue is full, it just ignores the hit 
    // rather than blocking your rhythm logic.
    xQueueSend(audioQueue, &req, 0);
}