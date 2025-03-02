//Configure.cpp

#include <Arduino.h>
#include "Configure.h"
#include "main.h"
#include "driver/i2s.h"
#include <driver/adc.h>
#include "max9814.h"

bool is_i2s_installed = false;

hw_timer_t *timer = NULL;

void configureI2SForSpeaker() {
  if(is_i2s_installed){
    i2s_driver_uninstall(I2S_NUM);
    is_i2s_installed = false;
  }
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),  // 主模式，发送数据
    .sample_rate = SAMPLE_RATE,                      // 采样率
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // 每个样本16位
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,         // 双声道（立体声）
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
  }
}

void configureI2SForMP3() {
  // 卸载之前的I2S驱动（如果已安装）
  if(is_i2s_installed){
    i2s_driver_uninstall(I2S_NUM);
    is_i2s_installed = false;
  }

  // 重新初始化I2S驱动
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 44100,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
      .bck_io_num = 26,
      .ws_io_num = 25,
      .data_out_num = 22,
      .data_in_num = I2S_PIN_NO_CHANGE
  };

  // 安装I2S驱动
  esp_err_t err = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  if (err == ESP_OK) {
      i2s_set_pin(I2S_NUM, &pin_config);
      is_i2s_installed = true;
  } else {
      Serial.println("Failed to install I2S driver");
  }
}

void configureADC() {
    // 配置 ADC 宽度为 12 位分辨率
    adc1_config_width(ADC_WIDTH_BIT_12);

    // 配置 ADC 衰减模式为 0dB，输入范围为 0-1.1V
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_6);
}

void configureTimer() {
    // 初始化定时器，每秒触发 SAMPLE_RATE 次采样
    timer = timerBegin(0, 80, true);  // 80 分频，相当于 1 MHz 的时钟频率
    buffer_index = BUFFER_SIZE;
    timerAttachInterrupt(timer, &onTimer, true);  // 绑定中断函数
    timerAlarmWrite(timer, 1000000 / SAMPLE_RATE, true);  // 每秒触发 SAMPLE_RATE 次
    timerAlarmEnable(timer);  // 启用定时器中断
}


void KeyConfig(){
    Serial.begin(115200);

    // 配置按键引脚为输入模式，并启用下拉电阻
    gpio_pad_select_gpio(BUTTON_GPIO);            // 选择 GPIO0
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);  // 配置为输入模式
    gpio_pulldown_en(BUTTON_GPIO);                // 启用下拉电阻

    // 配置 GPIO 中断
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_NEGEDGE);  // 配置为下降沿触发中断

    // 安装 GPIO 中断服务
    gpio_install_isr_service(0);  // 初始化中断服务，默认标志为 0
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);  // 添加中断处理函数
}

void IRAM_ATTR button_isr_handler(void* arg) {
    // 按键按下时执行的代码
    // 这里可以设置一个标志位，或者直接执行一些操作

    KeyNum |= 0x01;
    Serial.println("Button pressed!");
}