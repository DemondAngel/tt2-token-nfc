#ifndef STORE_DATA_H
#define STORE_DATA_H
#include <SharedKey.h>
#include <DeviceInfo.h>
#include <Arduino.h>
#include <ArduinoJson.h>

JsonDocument parseJsonFromFile(char * fileName);

void parseJsonToFile(JsonDocument doc, char * fileName);

bool activateSD();
void deactivateSD();

DeviceInfo getDeviceInfo();
void storeDeviceInfo(DeviceInfo deviceInfo);

//SharedKey getSingleSharedKey(char * uuid_shared_key);
//SharedKey * getSharedKeys();
//void writeSharedKey(SharedKey sharedKey);

//Crear métodos para recuperar usuario y contrasenia del lector

#endif
