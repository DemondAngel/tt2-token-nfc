#ifndef CIPHER_NFC_H
#define CIPHER_NFC_H

#include <Arduino.h>

void printHex(const char *label, const uint8_t *data, size_t len); //Testing purposes
size_t addPadding(uint8_t *data, size_t length);
int removePadding(uint8_t *data, size_t &len);
uint8_t * encryptNFCData(char * message);
char * decryptNFCData(uint8_t * data);

#endif