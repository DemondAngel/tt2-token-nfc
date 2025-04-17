#ifndef CARD_H
#define CARD_H

#include <Arduino.h>

const size_t CARD_UUID_LENGTH = 33;   // 32 bytes + null terminator
const size_t TOKEN_UUID_LENGTH = 33;  // 32 bytes + null terminator
const size_t CARD_TOKEN_LENGTH = 513;  // 512 bytes + null terminator

class Card {
private:
    char _uuidCard[CARD_UUID_LENGTH];
    char _uuidToken[TOKEN_UUID_LENGTH];
    char _token[CARD_TOKEN_LENGTH];

public:
    // Constructor
    Card();
    Card(const char* uuidCard, const char* uuidToken, const char* token);

    void setUuidCard(const char* uuidCard);
    char* getUuidCard();

    void setUuidToken(const char* uuidToken);
    char* getUuidToken();

    bool setToken(const char* token); // El setter ahora devuelve un bool para indicar éxito/fracaso
    char* getToken();
};

#endif