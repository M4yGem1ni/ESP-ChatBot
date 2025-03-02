//Wifi.cpp
#include <Arduino.h>
#include <WiFi.h>
#include <MyWifi.h>
#include "main.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

void Wifi_Init(const char* ssid, const char* password) {
    // 开始WiFi连接
    WiFi.begin(ssid, password);

    uint8_t cnt = 0;
    // 等待WiFi连接成功
    while (WiFi.status() != WL_CONNECTED) {
        if(cnt>5){
            esp_restart();
        }
        delay(1000);
        Serial.println("Connecting to WiFi...");
        cnt++;
    }

    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void Wifi_SeverTest(){
    HTTPClient http;
    http.begin(host);
    int httpCode = http.GET();
    if (httpCode > 0) {
        Serial.printf("HTTP GET 请求成功，返回值: %d\n", httpCode);
    } else {
        Serial.printf("HTTP GET 请求失败，错误码: %d\n", httpCode);
    }
    http.end();
}


// 上传WAV文件头和数据到服务器
bool Wifi_Upload(WAV_FILE* WAVfilePtr) {
    if (WiFi.status() == WL_CONNECTED) {  // 检查Wi-Fi连接状态
        Serial.println("Wifi Connected!");
    }
    else{
        Serial.println("Wifi Connect Failed!");
        return false;
    }
    HTTPClient http;
    http.begin(host+uploadUrl);
    http.setTimeout(30000); //  设置等待时间最大为30s
    http.addHeader("Content-Type", "audio/wav");
    size_t wavFileSize = sizeof(WAVHeader) + SAMPLE_RATE * SAMPLE_TIME * sizeof(int16_t);
    int httpResponseCode = http.sendRequest("POST",(uint8_t*)WAVfilePtr,wavFileSize);
    if (httpResponseCode > 0) {
        Serial.printf("POST request sent. Response code: %d\n", httpResponseCode);
        String response = http.getString();
        
        Serial.println("Response: " + response);
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, response);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }
        if (httpResponseCode != HTTP_CODE_OK){
            return false;
        }
        const char* status = doc["status"];  // 提取 "status" 参数
        const char* download_url = doc["download_url"];  // 提取 "download_url" 参数

        Serial.print("Download URL: ");
        Serial.println(download_url);
        downloadUrl = String(download_url);

    } else {
        Serial.printf("Error on sending POST: %d\n", httpResponseCode);
        return false;
    }http.end();
    return true;
}



// 下载文件到内存 (使用 host/downloadUrl)
int Wifi_Download(uint8_t** DownloadPtr,String download_url){
    HTTPClient http;
    http.begin(host+download_url);
    int httpCode = http.GET();  // 发起 GET 请求

    if(httpCode>0){
        if(httpCode == HTTP_CODE_OK){
            int fileSize = http.getSize();
            if(fileSize>0){
                *DownloadPtr = (uint8_t*)ps_malloc(fileSize);
                if (*DownloadPtr == NULL) {
                    Serial.println("Failed to allocate memory");
                    return -1;  // 返回错误码
                }
            
                WiFiClient* stream = http.getStreamPtr();
                int bytesRead = 0;
                while (http.connected() && (bytesRead < fileSize)) {
                    bytesRead += stream->readBytes(*DownloadPtr + bytesRead, fileSize - bytesRead);
                }
                Serial.printf("Downloaded %d bytes\n", bytesRead);
                dowmloadBytes = bytesRead;
                http.end();
                return fileSize;  // 返回文件大小
            }else{
                Serial.println("Invalid file size");
                http.end();
                return -1;
            }
        }else{
        Serial.printf("Failed to download file, HTTP code: %d\n", httpCode);
        http.end();
        return -1;
        }
    }else {
        Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return -1;
    }
    http.end();
}

