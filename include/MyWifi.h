//Wifi.h
#ifndef __MYWIFI_H
#define __MYWIFI_H

#include <Arduino.h>
#include "main.h"

void Wifi_Init(const char* ssid, const char* password);
bool Wifi_Upload(WAV_FILE* WAVfilePtr);
int Wifi_Download(uint8_t** DownloadPtr,String download_url);
void Wifi_SeverTest();

#endif
