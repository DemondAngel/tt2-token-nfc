#ifndef SHARED_KEY_H
#define SHARED_KEY_H

#include <Arduino.h>

class SharedKey {
    private:
        String _uuidSharedKey;
        String _sharedKey;

    public:
        SharedKey(String uuidSharedKey, String sharedKey);
        SharedKey();
        String getUuidSharedKey();
        void setUuidSharedKey(String uuidSharedKey);
        String getSharedKey();
        void setSharedKey(String sharedKey);
};

#endif