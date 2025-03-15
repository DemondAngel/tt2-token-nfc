#ifndef CIPHER_NFC_H
#define CIPHER_NFC_H

#include <Arduino.h>

void printHex(const char *label, const uint8_t *data, size_t len); //Testing purposes
size_t addPadding(uint8_t *data, size_t length);
int removePadding(uint8_t *data, size_t &len);
uint8_t * encryptNFCDATA(char * message);
char * decryptNFCDATA(uint8_t * data);

#endif