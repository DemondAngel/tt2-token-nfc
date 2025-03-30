#ifndef SHARED_KEY_H
#define SHARED_KEY_H

#include <Arduino.h>

const size_t UUID_SHARED_KEY_LENGTH = 33; // 32 bytes + null terminator
const size_t SHARED_KEY_LENGTH = 33;    // 32 bytes + null terminator
const size_t IV_LENGTH = 17;             // 32 bytes + null terminator

class SharedKey {
private:
    char _uuidSharedKey[UUID_SHARED_KEY_LENGTH];
    char _sharedKey[SHARED_KEY_LENGTH];
    char _iv[IV_LENGTH];

public:
    // Constructor por defecto
    SharedKey();

    // Constructor con parámetros (ahora acepta const char*)
    SharedKey(const char* uuidSharedKey, const char* sharedKey, const char* iv);

    // Destructor (no es necesario liberar memoria para arrays estáticos)
    ~SharedKey() = default;

    // Getter y Setter para uuidSharedKey
    void setUuidSharedKey(const char* uuidSharedKey);
    const char* getUuidSharedKey() const;

    // Getter y Setter para sharedKey
    void setSharedKey(const char* sharedKey);
    const char* getSharedKey() const;

    void setIv(const char* iv);
    const char* getIv() const;

    String toString() const;
};

#endif