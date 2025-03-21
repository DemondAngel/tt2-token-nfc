#ifndef CUSTOM_REQUESTS_H
#define CUSTOM_REQUESTS_H

#include <Card.h>
#include <SharedKey.h>
#include <Arduino.h>
#include <ArduinoJson.h>

JsonDocument parseJson();
bool connectToProxy();
String retrieveResponse();
String retrieveToken();
void storeToken(String token);
const char * registerCard();
const char * authNFC(String username, String pass);
Card generateToken(Card card, String uuid_nfc);
bool validateToken(Card card);
SharedKey getSharedKeyNFC(String uuid_shared_key);
void disconnect();

#endif