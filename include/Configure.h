//Configure.h
#ifndef __CONFIGURE_H
#define __CONFIGURE_H

extern bool is_i2s_installed;

void configureI2SForMP3();
void configureI2SForSpeaker();
void configureADC();
void configureTimer();
void KeyConfig();
void IRAM_ATTR button_isr_handler(void* arg);

#endif
