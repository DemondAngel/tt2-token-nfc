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
        uint8_t * getData();

        void setLen(size_t len);
        const size_t getLen();

};

class KeysInformation {
    private:

        uint8_t aesKey[17];  // Puntero al mensaje cifrado

    public:

        KeysInformation();

        // Constructor para inicializar la estructura
        KeysInformation(const SharedKey& sharedKey);

        // Destructor para liberar la memoria
        ~KeysInformation() = default;

        void setAesKey(const uint8_t * aesKey);
        const uint8_t * getAesKey();

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
        EncryptedMessage getEncryptedMessage();
        const char * decryptNFCData();
};

#endif