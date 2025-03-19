#ifndef SHARED_KEY_H
#define SHARED_KEY_H

#include <Arduino.h>

class SharedKey {
    private:
        String _uuid_shared_key;
        String _shared_key;

    public:
        SharedKey(String uuid_shared_key, String shared_key);
        String getUuidSharedKey();
        void setUuidSharedKey(String uuid_shared_key);
        String getSharedKey();
        void setSharedKey(String sharedKey);

};

#endif