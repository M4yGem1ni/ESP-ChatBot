//main.cpp
#include <Arduino.h>
#include "main.h"
#include "MyWifi.h"
#include "max9814.h"
#include "max98357A.h"
#include "Configure.h"
#include "wav.h"
#include "LED.h"
#include "FileExplorer.h"
#include "MyBluetooth.h"

String ssid = "your_ssid_here";
String passwd = "your_passwd_here";


String host = "your_server_here";
String downloadUrl;
String uploadUrl = "/upload";

WAV_FILE* wavfile_ptr;

uint8_t KeyNum = 0x00;

uint8_t* downloadfile;
size_t dowmloadBytes;

bool switcher;

void setup() {
    Serial.begin(115200);
    // wavfile_ptr = (WAV_FILE*)malloc(sizeof(WAV_FILE));
    monitorMemory();
    KeyConfig();
    motor_init();
    LED_Init();
    LED_Set(LED_PIN0,true);
    delay(100);
    LED_Set(LED_PIN0,false);
    
    configureADC();   // 配置 ADC
    configureTimer(); // 配置定时器以触发 ADC 采样
    configureI2SForSpeaker();

    Check_psram();
    // Check_psram();
    // MyBluetoothConfig();
    /*
    Wifi_Init(ssid.c_str(),passwd.c_str());
    Serial.println("UploadUrl:"+host+uploadUrl);
    Wifi_SeverTest();
    
    Serial.printf("Size of WAVHeader:%d\r\n",WAV_HEADER_SIZE);

    ClearAudioBuffer();
    Serial.println("开始采集音频数据...");
    delay(SAMPLE_TIME*1000+100);
    if (CheckBufferFull()) {  // 如果缓冲区已满，处理音频数据
        Serial.println("音频采集完成，开始处理音频数据...");
        // ClearAudioBuffer(); // 重新开始下一次采集
        // 在此处可以处理音频数据（例如传输、存储或分析）
        playAudioBuffer(wavfile_ptr->audio_buffer,SAMPLE_RATE*SAMPLE_TIME);
    }
    WriteWAVHeader(SAMPLE_RATE*SAMPLE_TIME,&(wavfile_ptr->WAV_Header));
    Serial.println("WAV Header:");
    Serial_File((uint8_t*)wavfile_ptr,128);

    
    if(Wifi_Upload(wavfile_ptr)){
        free(wavfile_ptr);
        monitorMemory();
        Wifi_Download(&downloadfile,downloadUrl);
        monitorMemory();
    }
    else{
        free(wavfile_ptr);
        Serial.println("Upload Failed!");
    }

    Serial_File(downloadfile,128);
    switcher = true;
    playWavfile(downloadfile,dowmloadBytes);
    // mp3_play_task(downloadfile,dowmloadBytes);
    switcher = true;
    */
    
}

void loop() {
    // free(wavfile_ptr);
    if(KeyNum&0x01){
        KeyNum = 0x00;
        wavfile_ptr = (WAV_FILE*)ps_malloc(sizeof(WAV_FILE));
        monitorMemory();
        Wifi_Init(ssid.c_str(),passwd.c_str());
        Serial.println("UploadUrl:"+host+uploadUrl);
        Wifi_SeverTest();
        
        Serial.printf("Size of WAVHeader:%d\r\n",WAV_HEADER_SIZE);

        ClearAudioBuffer();
        LED_Set(LED_PIN1,true);
        Serial.println("开始采集音频数据...");
        delay(SAMPLE_TIME*1000+100);
        if (CheckBufferFull()) {  // 如果缓冲区已满，处理音频数据
            Serial.println("音频采集完成，开始处理音频数据...");
            LED_Set(LED_PIN1,false);
            // ClearAudioBuffer(); // 重新开始下一次采集
            // 在此处可以处理音频数据（例如传输、存储或分析）
            // playAudioBuffer(wavfile_ptr->audio_buffer,SAMPLE_RATE*SAMPLE_TIME);
        }else{
            esp_restart();
        }
        WriteWAVHeader(SAMPLE_RATE*SAMPLE_TIME,&(wavfile_ptr->WAV_Header));
        Serial.println("WAV Header:");
        Serial_File((uint8_t*)wavfile_ptr,128);

        
        if(Wifi_Upload(wavfile_ptr)){
            free(wavfile_ptr);
            monitorMemory();
            Wifi_Download(&downloadfile,downloadUrl);
            monitorMemory();
            Serial_File(downloadfile,128);
            switcher = true;
            playWavfile(downloadfile,dowmloadBytes);
            // mp3_play_task(downloadfile,dowmloadBytes);
            switcher = true;
            free(downloadfile);
        }
        else{
            free(wavfile_ptr);
            Serial.println("Upload Failed!");
        }
    }
    delay(100);
}
