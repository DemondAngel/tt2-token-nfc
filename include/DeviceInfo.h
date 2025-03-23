#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <Arduino.h>
#include <SharedKey.h>

class DeviceInfo {
    private:
        String _uuid;
        String _userName;
        String _pass;
        String _token;
        SharedKey _sharedKey;
    public:
    // Constructor
        DeviceInfo(String uuid, String userName, String pass, String token, SharedKey sharedKey);

        void setUuid(String uuid);
        String getUuid();

        // Métodos Getters y Setters
        void setUserName(String userName);
        String getUserName();

        void setPass(String pass);
        String getPass();

        void setToken(String token);
        String getToken();

        void setSharedKey(SharedKey sharedKey);
        SharedKey getSharedKey();
    
};


#endif