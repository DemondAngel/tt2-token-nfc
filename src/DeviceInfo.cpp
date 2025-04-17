#include "DeviceInfo.h"

DeviceInfo::DeviceInfo() {
    _uuid[0] = '\0';
    _userName[0] = '\0';
    _pass[0] = '\0';
    _token[0] = '\0';
    // Si SharedKey tiene un constructor vacío, se puede inicializar de esta manera
}

// Constructor
DeviceInfo::DeviceInfo(const char* uuid, const char* userName, const char* pass, const char* token, SharedKey& sharedKey)
    : _sharedKey(sharedKey) {
    strncpy(_uuid, uuid, UUID_LENGTH - 1);
    _uuid[UUID_LENGTH - 1] = '\0';
    strncpy(_userName, userName, USERNAME_LENGTH - 1);
    _userName[USERNAME_LENGTH - 1] = '\0';
    strncpy(_pass, pass, PASS_LENGTH - 1);
    _pass[PASS_LENGTH - 1] = '\0';
    strncpy(_token, token, DEVICE_TOKEN_LENGTH - 1);
    _token[DEVICE_TOKEN_LENGTH - 1] = '\0';
}

// Setter para UUID
void DeviceInfo::setUuid(const char* uuid) {
    strncpy(_uuid, uuid, UUID_LENGTH - 1);
    _uuid[UUID_LENGTH - 1] = '\0';
}

// Getter para UUID
const char* DeviceInfo::getUuid() const {
    return _uuid;
}

// Setter para UserName
void DeviceInfo::setUserName(const char* userName) {
    strncpy(_userName, userName, USERNAME_LENGTH - 1);
    _userName[USERNAME_LENGTH - 1] = '\0';
}

// Getter para UserName
const char* DeviceInfo::getUserName() const {
    return _userName;
}

// Setter para Pass
void DeviceInfo::setPass(const char* pass) {
    strncpy(_pass, pass, PASS_LENGTH - 1);
    _pass[PASS_LENGTH - 1] = '\0';
}

// Getter para Pass
const char* DeviceInfo::getPass() const {
    return _pass;
}

// Setter para Token (devuelve bool para indicar éxito/fracaso)
bool DeviceInfo::setToken(const char* token) {
    if (strlen(token) < DEVICE_TOKEN_LENGTH) {
        strncpy(_token, token, DEVICE_TOKEN_LENGTH - 1);
        _token[DEVICE_TOKEN_LENGTH - 1] = '\0';
        return true; // Éxito
    } else {
        // El token es demasiado largo para el buffer
        _token[0] = '\0'; // O podrías truncar, dependiendo de la necesidad
        return false; // Fracaso
    }
}

// Getter para Token
const char* DeviceInfo::getToken() const {
    return _token;
}

// Setter para SharedKey (se pasa y almacena por valor)
void DeviceInfo::setSharedKey(const SharedKey& sharedKey) {
    _sharedKey = sharedKey; // La copia se realiza automáticamente
}

// Getter para SharedKey
SharedKey & DeviceInfo::getSharedKey() {
    return _sharedKey; // Devuelve una copia del objeto SharedKey
}

// Método toString() (como se definió anteriormente)
String DeviceInfo::toString() const {
    String output = "DeviceInfo: { ";
    output += "UUID: ";
    output += _uuid;
    output += ", UserName: ";
    output += _userName;
    output += ", Pass: *****"; // No mostrar la contraseña por seguridad
    output += ", Token: ";
    output += _token;
    output += ", SharedKey: { UUID: ";
    output += _sharedKey.getUuidSharedKey();
    output += ", Key: *****, IV: ";
    output += " } }";
    return output;
}