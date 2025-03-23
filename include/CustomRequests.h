#ifndef CUSTOM_REQUESTS_H
#define CUSTOM_REQUESTS_H

#include <Card.h>
#include <DeviceInfo.h>
#include <Arduino.h>
#include <ArduinoJson.h>

JsonDocument parseJson();
bool connectToProxy();
const char * registerCard();
DeviceInfo authNFC(String userName, String pass);
Card generateToken(Card card);
bool validateToken(Card card);
SharedKey getSharedKeyNFC(String uuid_shared_key);
void disconnect();

#endif