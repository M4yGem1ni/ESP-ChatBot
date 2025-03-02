//FileExplorer.cpp
#include "main.h"
#include <SPIFFS.h>
#include <FS.h>

bool Check_psram(){
    if (psramFound()) {
        Serial.print("Total PSRAM: ");
        Serial.print(ESP.getPsramSize() / 1024);
        Serial.println(" KB");
        Serial.print("Free PSRAM: ");
        Serial.print(ESP.getFreePsram() / 1024);
        Serial.println(" KB");
        return true;
    } else {
        Serial.println("No PSRAM detected.");
        return false;
    }
}

void* PsramMalloc(size_t size){
    void* ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    if (ptr != nullptr) {
        Serial.println("PSRAM 内存分配成功");
    } else {
        Serial.println("PSRAM 内存分配失败");
    }
    return ptr;
}

//将文件保存到SRAM
bool SaveFile(const char* filePath,WAV_FILE* wavfilePtr) {
    // 初始化 SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS 初始化失败");
        return false;
    }

    // 打开文件以写入
    File file = SPIFFS.open(filePath, FILE_WRITE);
    if (!file) {
        Serial.println("无法打开文件进行写入");
        return false;
    }

    // 写入WAV文件
    file.write((const uint8_t*)wavfilePtr, sizeof(WAV_FILE));
    
    Serial.printf("成功保存 WAV 文件到: %s\n", filePath);
    
    // 关闭文件
    file.close();

    return true;
}

void Serial_File(uint8_t* filePtr,size_t Byte){
    size_t i;
    for(i=0;i<Byte;i++){
        Serial.printf("%02x ",*(filePtr+i));
    }
    Serial.printf("\r\n");
}

void monitorMemory() {
    Serial.print("Free heap: ");
    Serial.println(esp_get_free_heap_size());
}

