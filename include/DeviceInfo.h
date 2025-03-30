#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <Arduino.h>
#include <SharedKey.h> // Asegúrate de que la ruta a SharedKey.h sea correcta

const size_t UUID_LENGTH = 33;       // 32 bytes + null
const size_t USERNAME_LENGTH = 21;   // 20 bytes + null
const size_t PASS_LENGTH = 21;       // 20 bytes + null
const size_t DEVICE_TOKEN_LENGTH = 513; // 512 bytes + null

class DeviceInfo {
    private:
        char _uuid[UUID_LENGTH];
        char _userName[USERNAME_LENGTH];
        char _pass[PASS_LENGTH];
        char _token[DEVICE_TOKEN_LENGTH];
        SharedKey _sharedKey;

    public:
        // Constructor
        DeviceInfo();
        DeviceInfo(const char* uuid, const char* userName, const char* pass, const char* token, const SharedKey& sharedKey);

        // Destructor (SharedKey no gestiona memoria dinámica en la versión estática)
        ~DeviceInfo() = default;

        // Setter y Getter para UUID
        void setUuid(const char* uuid);
        const char* getUuid() const;

        // Setter y Getter para UserName
        void setUserName(const char* userName);
        const char* getUserName() const;

        // Setter y Getter para Pass
        void setPass(const char* pass);
        const char* getPass() const;

        // Setter y Getter para Token
        bool setToken(const char* token); // Setter devuelve bool para indicar éxito/fracaso
        const char* getToken() const;

        // Setter y Getter para SharedKey (se pasa y almacena por valor)
        void setSharedKey(const SharedKey& sharedKey);
        SharedKey getSharedKey() const;

        String toString() const;
};

#endif