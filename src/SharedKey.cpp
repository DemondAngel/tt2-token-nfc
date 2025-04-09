#include <SharedKey.h>
#include <string.h>
#include <Arduino.h> // Para Serial (opcional, para depuración)

SharedKey::SharedKey() {
    _uuidSharedKey[0] = '\0';
    _sharedKey[0] = '\0';
}

SharedKey::SharedKey(const char* uuidSharedKey, const char* sharedKey) {
    setUuidSharedKey(uuidSharedKey);
    setSharedKey(sharedKey);
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

String SharedKey::toString() const {
    String output = "UUID Shared Key: ";
    output += _uuidSharedKey;
    output += ", Shared Key: ";
    output += _sharedKey;
    return output;
}