#ifndef CIPHER_NFC_H
#define CIPHER_NFC_H

#include <Arduino.h>
#include <SharedKey.h>

class EncryptedMessage {
    private:
        uint8_t _data[513];
        size_t _len;
    
    public:
        EncryptedMessage();
        EncryptedMessage(const uint8_t * data, size_t len);
        ~EncryptedMessage() = default;

        void setData(const uint8_t *data, size_t len);
        const uint8_t * getData();

        void setLen(size_t len);
        const size_t getLen();

};

class KeysInformation {
    private:

        uint8_t aesKey[33];  // Puntero al mensaje cifrado
        uint8_t aesIv[17];  // Puntero al mensaje cifrado

    public:

        KeysInformation();

        // Constructor para inicializar la estructura
        KeysInformation(const SharedKey& sharedKey);

        // Destructor para liberar la memoria
        ~KeysInformation() = default;

        void setAesKey(const uint8_t * aesKey);
        const uint8_t * getAesKey();

        void setAesIv(const uint8_t * aesKey);
        const uint8_t * getAesIv();
};

class EncryptionNFC{
    private:
        KeysInformation _keysInformation;
        EncryptedMessage _encryptedMessage;
    public:
        EncryptionNFC();
        EncryptionNFC(const KeysInformation& keysInformation, const EncryptedMessage& encryptedMessage);

        void printHex(const char *label, const uint8_t *data, const size_t len);
        void encryptNFCData(const char * message);
        const char * decryptNFCData();
};

#endif