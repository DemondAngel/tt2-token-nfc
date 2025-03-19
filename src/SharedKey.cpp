#include <SharedKey.h>

SharedKey::SharedKey(String uuid_shared_key, String shared_key){
    this->_uuid_shared_key = uuid_shared_key;
    this->_shared_key = shared_key;
}

String SharedKey::getUuidSharedKey(){
    return this->_uuid_shared_key = _uuid_shared_key;
}

void SharedKey::setUuidSharedKey(String uuid_shared_key){
    this->_uuid_shared_key = uuid_shared_key;
}

String SharedKey::getSharedKey(){
    return this->_shared_key;
}

void SharedKey::setSharedKey(String sharedKey){
    this->_shared_key = sharedKey;
}