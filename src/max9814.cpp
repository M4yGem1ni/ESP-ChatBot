//max9814.cpp
#include <Arduino.h>
#include <driver/adc.h>
#include "Configure.h"
#include "main.h"

volatile int buffer_index = 0;      // 当前缓冲区索引

void motor_init() {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;  // 禁用中断
    io_conf.mode = GPIO_MODE_OUTPUT;        // 配置为输出模式
    io_conf.pin_bit_mask = (1ULL << MOTOR_GPIO_PIN);  // 指定要配置的引脚
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;     // 禁用下拉
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;         // 启用上拉
    gpio_config(&io_conf);                            // 应用配置
}

// 启动电机
void motor_start() {
    gpio_set_level((gpio_num_t)MOTOR_GPIO_PIN, 1);  // 设置 GPIO 为高电平，启动电机
}

// 停止电机
void motor_stop() {
    gpio_set_level((gpio_num_t)MOTOR_GPIO_PIN, 0);  // 设置 GPIO 为低电平，停止电机
}

// 中断处理函数：用于定时触发 ADC 采样
void IRAM_ATTR onTimer() 
{
    static bool switch0 = true;
    // 获取 ADC 采样值并存入缓冲区
    if (buffer_index < BUFFER_SIZE) {
        int adc_value = adc1_get_raw(ADC1_CHANNEL_6);  // 读取 ADC 值
        wavfile_ptr->audio_buffer[buffer_index++] = adc_value;      // 存入音频缓冲区
    }
    if (switcher) {
        if(switch0){
            motor_start();  // 假设这是一个启动电机的函数
            switch0 = !switch0;
            switcher = false;
        }else {
            motor_stop();   // 假设这是一个停止电机的函数
            switch0 = !switch0;
            switcher = false;
        } 
    }
}

uint8_t CheckBufferFull(void){
    if(buffer_index >= BUFFER_SIZE){
        return 1;
    }else{
        return 0;
    }
}

void ClearAudioBuffer(void){
    buffer_index = 0;
}