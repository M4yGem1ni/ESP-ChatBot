//LED.cpp
#include "main.h"
#include <Arduino.h>

//  配置为开漏输出模式

void LED_Init(){
    pinMode(LED_PIN0, OUTPUT);
    pinMode(LED_PIN1, OUTPUT);

    // 初始化时熄灭 LED
    digitalWrite(LED_PIN0, LOW);  // LED1 关闭
    digitalWrite(LED_PIN1, LOW);  // LED2 关闭
}

void LED_Set(int LED_PIN,bool Status){
    digitalWrite(LED_PIN, Status?1:0);  // LED1 亮
}