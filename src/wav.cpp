#include "wav.h"
#include <Arduino.h>

void WriteWAVHeader(size_t totalSamples,WAVHeader* header) {
    header->riff_header[0] = 'R';
    header->riff_header[1] = 'I';
    header->riff_header[2] = 'F';
    header->riff_header[3] = 'F';
    
    header->wav_size = totalSamples * sizeof(int16_t) + sizeof(WAVHeader) - 8;
    
    header->wave_header[0] = 'W';
    header->wave_header[1] = 'A';
    header->wave_header[2] = 'V';
    header->wave_header[3] = 'E';

    header->fmt_header[0] = 'f';
    header->fmt_header[1] = 'm';
    header->fmt_header[2] = 't';
    header->fmt_header[3] = ' ';

    header->fmt_chunk_size = 16;
    header->audio_format = 1;
    header->num_channels = NUM_CHANNELS;
    header->sample_rate = SAMPLE_RATE;
    header->byte_rate = SAMPLE_RATE * BITS_PER_SAMPLE * NUM_CHANNELS / 8;
    header->block_align = BITS_PER_SAMPLE * NUM_CHANNELS / 8;
    header->bits_per_sample = BITS_PER_SAMPLE;

    header->data_header[0] = 'd';
    header->data_header[1] = 'a';
    header->data_header[2] = 't';
    header->data_header[3] = 'a';

    header->data_bytes = totalSamples * sizeof(int16_t);
}

