// MyBluetooth.cpp
#include <Arduino.h>
#include "main.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *pCharacteristic;

bool deviceConnected = false;
char* receivedData = (char*)malloc(sizeof(char)*50);
int receivedIndex = 0;

// 服务和特性 UUID
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// 创建蓝牙串行对象

void modifyWifiConfig(char* bhost,char* bssid, char* bpasswd){
    host = bhost;
    ssid = bssid;
    passwd = bpasswd;
    Serial.println(host);
    Serial.println(ssid);
    Serial.println(passwd);
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        
        if (value.length() > 0) {
            Serial.print("Received: ");
            Serial.println(value.c_str());
            
            // 复制接收到的数据
            strcpy(receivedData, value.c_str());
            receivedIndex++;
        }
    }
};

void MyBluetoothConfig(){
    Serial.begin(115200);

    char bhost[50];
    char bssid[50];
    char bpasswd[50];

    // 初始化 BLE
    BLEDevice::init("ESP32_BLE_Device");
    BLEServer *pServer = BLEDevice::createServer();

    // 创建 BLE 服务
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // 创建 BLE 特性
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE
                      );

    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new MyCallbacks());

    // 启动服务
    pService->start();

    // 开始广告，使设备可被发现
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();

    Serial.println("Waiting for data...");

    // 持续监测数据
    receivedIndex = 0;
    while (receivedIndex < 3) {  // 等待接收 host、ssid 和 passwd
        if (receivedIndex == 1) {
            strcpy(bhost, receivedData);
            Serial.print("Host received: ");
            Serial.println(bhost);
            receivedData = bssid;  // 准备接收 SSID
            Serial.println("Waiting for SSID...");
        } else if (receivedIndex == 2) {
            strcpy(bssid, receivedData);
            Serial.print("SSID received: ");
            Serial.println(bssid);
            receivedData = bpasswd;  // 准备接收密码
            Serial.println("Waiting for Password...");
        } else if (receivedIndex == 3) {
            strcpy(bpasswd, receivedData);
            Serial.print("Password received: ");
            Serial.println(bpasswd);
            break;  // 接收完成，退出循环
        }

        delay(500);  // 短暂延时，防止过快检查
    }

    Serial.println("All data received.");
    modifyWifiConfig(bhost,bssid,bpasswd);
}