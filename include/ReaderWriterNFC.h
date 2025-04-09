#ifndef READER_WRITER_NFC_H
#define READER_WRITER_NFC_H

#include <Arduino.h>
#include <Adafruit_PN532.h>

const int SECTOR_UUID_CARD = 0;
const int SECTOR_UUID_TOKENS_VERSION = 1;
const int SECTOR_UUID_SHARED_KEY = 2;
const int INIT_SECTOR_TOKEN = 3;
const int FINAL_SECTOR = 16;

#if TARGET_PLATFORM == 0
  const int SDA_PIN = 20;
  const int SCL_PIN = 21;
#else
  const int SDA_PIN = 21;
  const int SCL_PIN = 22;
#endif


class ReaderWriter {
    private:
        Adafruit_PN532 _nfc;
        uint8_t _key[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        int writeBlock(int block, uint8_t* dataBytes, uint8_t* uid, uint8_t uidLength);
        uint8_t * readBlock(int block, uint8_t * uid, uint8_t uidLength);
        int _writeUuidGeneric(const char * uuid, int sector);
        
    public:
        ReaderWriter(Adafruit_PN532 nfc);
        void initPN532();
        int writeUuidCard(const char * uuidCard);
        int writeUuidTokensVersion(const char * uuidTokensVersion);
        int writeUuidSharedKey(const char * uuidSharedKey);
        int writeToken(uint8_t * token, size_t lengthToken);
        void readToken(void);
        bool detectCard(void);

};

#endif