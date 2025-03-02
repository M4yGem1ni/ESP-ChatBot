//FileExplorer.h
#ifndef __FILEEXPLORER_H
#define __FILEEXPLORER_H

#include "main.h"
#include <SPIFFS.h>
#include <FS.h>

bool SaveFile(const char* filePath,WAV_FILE* wavfilePtr);
void Serial_File(uint8_t* filePtr,size_t Byte);
void monitorMemory();
bool Check_psram();

#endif
