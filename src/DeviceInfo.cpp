#include "DeviceInfo.h"

// Constructor
DeviceInfo::DeviceInfo(String uuid, String userName, String pass, String token, SharedKey sharedKey) {
    this->_uuid = uuid;
    this->_userName = userName;
    this->_pass = pass;
    this->_token = token;
    this->_sharedKey = sharedKey;
}

// Setter y Getter para UUID
void DeviceInfo::setUuid(String uuid) {
    this->_uuid = uuid;
}

String DeviceInfo::getUuid() {
    return this->_uuid;
}

// Setter y Getter para UserName
void DeviceInfo::setUserName(String userName) {
    this->_userName = userName;
}

String DeviceInfo::getUserName() {
    return this->_userName;
}

// Setter y Getter para Pass
void DeviceInfo::setPass(String pass) {
    this->_pass = pass;
}

String DeviceInfo::getPass() {
    return this->_pass;
}

// Setter y Getter para Token
void DeviceInfo::setToken(String token) {
    this->_token = token;
}

String DeviceInfo::getToken() {
    return this->_token;
}

void DeviceInfo::setSharedKey(SharedKey sharedKey){
    this->_sharedKey = sharedKey;
}

SharedKey DeviceInfo::getSharedKey(){
    return this->_sharedKey;
}