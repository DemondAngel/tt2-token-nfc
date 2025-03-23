#ifndef CARD_H
#define CARD_H

#include <Arduino.h>

class Card {
    private: 
        String _uuidCard;
        String _uuidToken;
        String _token;
    
    public:
        Card(String uuidCard, String uuidToken, String token);
    
        String getUuidCard();
        void setUuidCard(String uuidCard);
        String getUuidToken();
        void setUuidToken(String uuidToken);
        String getToken();
        void setToken(String token); 
};

#endif