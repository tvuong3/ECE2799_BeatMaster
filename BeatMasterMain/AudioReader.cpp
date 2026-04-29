#include "AudioReader.h"

#define VOLUME_CONTROL_PIN 39

I2SClass i2s;
float globalVolume = 0.2;

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

void play_wav_from_sd(const char* path) {
    File file = SD.open(path);
    if (!file) return;

    WavHeader header;
    if (file.read((uint8_t*)&header, 44) != 44) {
        Serial.println("Invalid WAV header");
        file.close();
        return;
    }

    // Determine the I2S bit depth to use. 
    // ESP32 I2S hardware works best with 16-bit or 32-bit slots.
    // 24-bit and 32-bit files will both be sent as 32-bit.
    i2s_data_bit_width_t i2sWidth;
    if (header.bitsPerSample > 16) {
        i2sWidth = I2S_DATA_BIT_WIDTH_32BIT;
    } else {
        i2sWidth = I2S_DATA_BIT_WIDTH_16BIT;
    }
    i2s_data_bit_width_t i2s_width = (header.bitsPerSample > 16) ? I2S_DATA_BIT_WIDTH_32BIT : I2S_DATA_BIT_WIDTH_16BIT;
    
    Serial.printf("Configuring I2S: %uHz, %u-bit file -> %u-bit I2S, %u channels\n", 
                  header.sampleRate, header.bitsPerSample, (i2sWidth == I2S_DATA_BIT_WIDTH_32BIT ? 32 : 16), header.channels);
    
    // Custom I2S setup call (Ensures slot width matches our processing)
    setup_i2s_for_file(header.sampleRate, i2sWidth, header.channels);

    uint8_t rawBuffer[BUFFER_SIZE];
    int32_t processedBuffer[BUFFER_SIZE / (header.bitsPerSample / 8)]; // Worst case: enough space for 32-bit samples

    // Getting volume from control pin
    float rawVolume = analogRead(VOLUME_CONTROL_PIN);
    Serial.print("Raw Volume: ");
    Serial.println(rawVolume);
    float adjustedVolume = rawVolume / 4096.0;
    Serial.print("Volume: ");
    Serial.println(adjustedVolume);
    // Serial.println();

    while (file.available()) {
        int bytesRead = file.read(rawBuffer, BUFFER_SIZE);
        int bytesPerSample = header.bitsPerSample / 8;
        int samplesCount = bytesRead / bytesPerSample;
        size_t bytesToWrite = 0;

        for (int i = 0; i < samplesCount; i++) {
            int32_t sample = 0;
            uint8_t* p = &rawBuffer[i * bytesPerSample];

            // --- 1. Parse Sample Based on Bit Depth ---
            if (header.bitsPerSample == 16) {
                // 16-bit is signed Little Endian
                sample = (int32_t)((int16_t)(p[1] << 8 | p[0]));
            } 
            else if (header.bitsPerSample == 24) {
                // 24-bit signed Little Endian -> Reconstruct and Sign Extend
                sample = (int32_t)((p[2] << 24) | (p[1] << 16) | (p[0] << 8));
                if (sample & 0x800000) sample |= 0xFF000000;
            } 
            else if (header.bitsPerSample == 32) {
                // 32-bit signed integer
                sample = (int32_t)((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
            }

            // --- 2. Apply Volume ---
            sample = (int32_t)(sample * adjustedVolume);

            // --- 3. Pack into the Output Buffer ---
            if (i2sWidth == I2S_DATA_BIT_WIDTH_16BIT) {
                ((int16_t*)processedBuffer)[i] = (int16_t)sample;
                bytesToWrite = samplesCount * 2;
            } else {
                processedBuffer[i] = sample;
                bytesToWrite = samplesCount * 4;
            }
        }

        // Write processed data to I2S
        i2s.write((uint8_t*)processedBuffer, bytesToWrite);
    }
    
    file.close();
    Serial.println("Playback complete.");
}