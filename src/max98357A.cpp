//max98357A.cpp
#include <Arduino.h>
#include <driver/i2s.h>
#include "main.h"
#include "max98357A.h"
#include "Configure.h"
#include "minimp3.h"
#include "FileExplorer.h"

mp3dec_t mp3d;
mp3dec_frame_info_t frame_info;
int16_t* pcm_buffer; //[MINIMP3_MAX_SAMPLES_PER_FRAME];  // 存储解码后的PCM数据


void playAudioBuffer8bit(int8_t *buf, size_t buffer_size) {
  size_t bytes_written;

  // 按照采样率发送数据
  for (size_t i = 0; i < buffer_size; i++) {
    int8_t sample = buf[i];  // 从缓冲区中读取一个采样值

    // 将采样值通过 I2S 发送到扬声器
    i2s_write(I2S_NUM, &sample, sizeof(sample), &bytes_written, portMAX_DELAY);
  }
}

void play_pcm(int16_t* data, size_t len) {
    size_t bytes_written;
    i2s_write(I2S_NUM, data, len * sizeof(int16_t), &bytes_written, portMAX_DELAY);
}

void playWavfile(uint8_t* wav_ptr,size_t Byte){
    WAVHeader* wavheader = (WAVHeader*)wav_ptr;

    // 检查 fmt_chunk_size 和 audio_format
    // if (wavheader->fmt_chunk_size != 16 || wavheader->audio_format != 1) {
    //     Serial.println("Unsupported WAV format");
    //     return;
    // }

    if(is_i2s_installed){
        i2s_driver_uninstall(I2S_NUM);
        is_i2s_installed = false;
    }
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),  // 主模式，发送数据
        .sample_rate = wavheader->sample_rate,                      // 采样率
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // 每个样本16位
        .channel_format = (wavheader->num_channels == 2) ? I2S_CHANNEL_FMT_RIGHT_LEFT : I2S_CHANNEL_FMT_ONLY_RIGHT,         // 双声道（立体声）
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,      // I2S格式
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,             // 中断等级1
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_PIN,    // BCK 引脚
        .ws_io_num = I2S_LRCK_PIN,    // LRC 引脚
        .data_out_num = I2S_DATA_PIN, // 数据输出引脚
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    esp_err_t err = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    if (err == ESP_OK) {
        i2s_set_pin(I2S_NUM, &pin_config);
        is_i2s_installed = true;
    } else {
        Serial.println("Failed to install I2S driver");
        return;
    }
    int16_t* audio_data = (int16_t*)(wav_ptr + sizeof(WAVHeader));
    size_t audio_data_size = wavheader->data_bytes / sizeof(int16_t);

    // Serial.println("num_channels:"+String(wavheader->num_channels));
    // Serial.println("bits_per_sample:"+String(wavheader->bits_per_sample));
    // Serial.println("block_align:"+String(wavheader->block_align));
    // Serial.println("byte_rate:"+String(wavheader->byte_rate));
    // Serial.println("data_bytes:"+String(wavheader->data_bytes));
    // Serial.println("data_header:"+String(wavheader->data_header));
    // Serial.println("fmt_chunk_size:"+String(wavheader->fmt_chunk_size));
    // Serial.println("fmt_header:"+String(wavheader->fmt_header));
    // Serial.println("riff_header:"+String(wavheader->riff_header));
    // Serial.println("sample_rate:"+String(wavheader->sample_rate));
    // Serial.println("wav_size:"+String(wavheader->wav_size));
    // Serial.println("wave_header:"+String(wavheader->wave_header));

    // Serial.println("audio_data_size"+String(audio_data_size));
    playAudioBuffer(audio_data,audio_data_size);
    configureI2SForSpeaker();
}



void playAudioBuffer(int16_t *buf, size_t buffer_size) {
  size_t bytes_written;

  // 按照采样率发送数据
  for (size_t i = 0; i < buffer_size; i++) {
    int16_t sample = buf[i];  // 从缓冲区中读取一个采样值

    // 将采样值通过 I2S 发送到扬声器
    i2s_write(I2S_NUM, &sample, sizeof(sample), &bytes_written, portMAX_DELAY);
  }
}

uint8_t* skip_id3_tag(uint8_t* mp3_data, size_t* mp3_size) {
    // 检查是否有ID3标签
    if (mp3_data[0] == 0x49 && mp3_data[1] == 0x44 && mp3_data[2] == 0x33) {
        // ID3标签存在，读取标签大小
        size_t tag_size = (mp3_data[6] << 21) | (mp3_data[7] << 14) | (mp3_data[8] << 7) | mp3_data[9];

        // 跳过ID3标签部分
        mp3_data += 10 + tag_size;
        *mp3_size -= (10 + tag_size);  // 更新音频数据的大小
    }

    return mp3_data;  // 返回跳过标签后的音频数据指针
}

void mp3_play_task(uint8_t* mp3_data, size_t Bytes) {
    configureI2SForMP3();
    pcm_buffer = (int16_t*)malloc(MINIMP3_MAX_SAMPLES_PER_FRAME);

    monitorMemory();

    // 跳过ID3标签部分
    mp3_data = skip_id3_tag(mp3_data, &Bytes);

    uint8_t* mp3_ptr = mp3_data;
    size_t mp3_bytes_remaining = Bytes;

    while (mp3_bytes_remaining > 0) {
        // 解码 MP3 数据
        int samples = mp3dec_decode_frame(&mp3d, mp3_ptr, mp3_bytes_remaining, pcm_buffer, &frame_info);
        if (samples > 0 && samples <= MINIMP3_MAX_SAMPLES_PER_FRAME) {
            // 播放解码后的PCM数据
            play_pcm(pcm_buffer, samples * frame_info.channels);
        }

        // 移动指针，继续解码下一个数据块
        if (frame_info.frame_bytes <= mp3_bytes_remaining) {
            mp3_ptr += frame_info.frame_bytes;
            mp3_bytes_remaining -= frame_info.frame_bytes;
        } else {
            // 如果 frame_bytes 超过剩余的 MP3 字节，直接退出循环以防止越界
            break;
        }

        // 喂食看门狗
        yield();  // 或者使用 taskYIELD(); 如果使用FreeRTOS
    }
    free(pcm_buffer);
    configureI2SForSpeaker();
}