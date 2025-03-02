//main.h
#ifndef __MAIN_H
#define __MAIN_H

#include "Arduino.h"

#define I2S_NUM         I2S_NUM_0  // 使用I2S端口0

//麦克风
#define MIC_PIN         6          // MAX9814麦克风连接的引脚 (ADC1通道6, GPIO34)

#define BUTTON_GPIO     GPIO_NUM_13  // 假设按键连接在 GPIO13

//扬声器
#define I2S_BCK_PIN     14         // BCLK引脚
#define I2S_LRCK_PIN    16         // LRC引脚
#define I2S_DATA_PIN    21         // 数据引脚

#define SAMPLE_RATE     16000       // 采样率（16kHz）
#define SAMPLE_TIME     6
#define BUFFER_SIZE     (SAMPLE_RATE * SAMPLE_TIME)  // 3秒的音频缓冲区大小
#define BITS_PER_SAMPLE  16    // 每个采样16位
#define NUM_CHANNELS     1     // 单声道

//电机
#define MOTOR_GPIO_PIN  12

//LED
#define LED_PIN0        4
#define LED_PIN1        5

#define WAV_HEADER_SIZE  44     // WAV 文件头大小（44 字节）

// 定义轮询间隔时间（例如1秒）
#define POLLING_INTERVAL 1000

// WAV 文件头结构
struct WAVHeader {
  char riff_header[4] = {'R', 'I', 'F', 'F'};   // "RIFF"
  uint32_t wav_size;                            // 文件大小 - 8 字节
  char wave_header[4] = {'W', 'A', 'V', 'E'};   // "WAVE"
  char fmt_header[4] = {'f', 'm', 't', ' '};    // "fmt "
  uint32_t fmt_chunk_size = 16;                 // PCM 格式下固定为 16
  uint16_t audio_format = 1;                    // PCM = 1
  uint16_t num_channels = NUM_CHANNELS;         // 声道数
  uint32_t sample_rate = SAMPLE_RATE;           // 采样率
  uint32_t byte_rate;                           // 每秒字节数 (采样率 * 位宽 * 声道数 / 8)
  uint16_t block_align;                         // 每个采样块大小 (位宽 * 声道数 / 8)
  uint16_t bits_per_sample = BITS_PER_SAMPLE;   // 每个采样的位数
  char data_header[4] = {'d', 'a', 't', 'a'};   // "data"
  uint32_t data_bytes;                          // 数据部分的字节数
};

struct WAV_FILE{
  WAVHeader WAV_Header;
  int16_t audio_buffer[BUFFER_SIZE];
};

extern String host;
extern String ssid;
extern String passwd;
extern String downloadUrl;
extern String uploadUrl;

extern bool switcher;

extern WAV_FILE* wavfile_ptr;

extern size_t dowmloadBytes;

extern uint8_t KeyNum;
// extern int16_t* audio_buffer;
// extern WAVHeader WAV_Header;

#endif
