//wav.h
#ifndef __WAV_H
#define __WAV_H

#include <Arduino.h>
#include "main.h"

void WriteWAVHeader(size_t totalSamples,WAVHeader* header);

#endif
