#ifndef READER_WRITER_NFC_H
#define READER_WRITER_NFC_H

#include <Arduino.h>

void initPN532();
int writeBlock(int block, uint8_t* dataBytes, uint8_t* uid, uint8_t uidLength);
uint8_t * readBlock(int block, uint8_t * uid, uint8_t uidLength);
int write(const char* mensaje);
void read(void);


#endif