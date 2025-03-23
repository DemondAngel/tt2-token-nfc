#include "SharedKey.h"

SharedKey::SharedKey() {}

SharedKey::SharedKey(String uuidSharedKey, String sharedKey) {
    this->_uuidSharedKey = uuidSharedKey;
    this->_sharedKey = sharedKey;
}

String SharedKey::getUuidSharedKey() {
    return this->_uuidSharedKey;
}

void SharedKey::setUuidSharedKey(String uuidSharedKey) {
    this->_uuidSharedKey = uuidSharedKey;
}

String SharedKey::getSharedKey() {
    return this->_sharedKey;
}

void SharedKey::setSharedKey(String sharedKey) {
    this->_sharedKey = sharedKey;
}