//max9814.h
#ifndef __MAX9814_H
#define __MAX9814_H

#include <Arduino.h>

extern volatile int buffer_index;

void motor_init();
void IRAM_ATTR onTimer();
uint8_t CheckBufferFull(void);
void ClearAudioBuffer(void);

#endif
