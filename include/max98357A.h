//max98357A.h
#ifndef __MAX98357A_H
#define __MAX98357A_H

#include <Arduino.h>

void playAudioBuffer(int16_t *buf, size_t buffer_size);
void mp3_play_task(uint8_t* mp3_data, size_t Bytes);
void playWavfile(uint8_t* wav_ptr,size_t Byte);

#endif
