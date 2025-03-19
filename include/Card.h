#ifndef CARD_H
#define CARD_H

#include <Arduino.h>

class Card {
    private: 
        String _uuid_card;
        String _uuid_token;
        String _token;
    
    public:
        Card(String uuid_card, String uuid_token, String token);

        String getUuidCard();
        void setUuidCard(String uuid_card);
        String getUuidToken();
        void setUuidToken(String uuid_token);
        String getToken();
        void setToken(String token);
};

#endif