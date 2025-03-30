#include <SharedKey.h>
#include <string.h>
#include <Arduino.h> // Para Serial (opcional, para depuración)

SharedKey::SharedKey() {
    _uuidSharedKey[0] = '\0';
    _sharedKey[0] = '\0';
    _iv[0] = '\0';
}

SharedKey::SharedKey(const char* uuidSharedKey, const char* sharedKey, const char* iv) {
    setUuidSharedKey(uuidSharedKey);
    setSharedKey(sharedKey);
    setIv(iv);
}

void SharedKey::setUuidSharedKey(const char* uuidSharedKey) {
    if (uuidSharedKey != nullptr) {
        strncpy(_uuidSharedKey, uuidSharedKey, UUID_SHARED_KEY_LENGTH - 1);
        _uuidSharedKey[UUID_SHARED_KEY_LENGTH - 1] = '\0';
    } else {
        _uuidSharedKey[0] = '\0';
    }
}

const char* SharedKey::getUuidSharedKey() const {
    return _uuidSharedKey;
}

void SharedKey::setSharedKey(const char* sharedKey) {
    if (sharedKey != nullptr) {
        strncpy(_sharedKey, sharedKey, SHARED_KEY_LENGTH - 1);
        _sharedKey[SHARED_KEY_LENGTH - 1] = '\0';
    } else {
        _sharedKey[0] = '\0';
    }
}

const char* SharedKey::getSharedKey() const {
    return _sharedKey;
}

void SharedKey::setIv(const char* iv) {
    if (iv != nullptr) {
        strncpy(_iv, iv, IV_LENGTH - 1);
        _iv[IV_LENGTH - 1] = '\0';
    } else {
        _iv[0] = '\0';
    }
}

const char* SharedKey::getIv() const {
    return _iv;
}

String SharedKey::toString() const {
    String output = "UUID Shared Key: ";
    output += _uuidSharedKey;
    output += ", Shared Key: ";
    output += _sharedKey;
    output += ", IV: ";
    output += _iv;
    return output;
}