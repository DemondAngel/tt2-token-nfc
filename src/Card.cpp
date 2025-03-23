#include "Card.h"

Card::Card(String uuidCard, String uuidToken, String token) {
    this->_uuidCard = uuidCard;
    this->_uuidToken = uuidToken;
    this->_token = token;
}

String Card::getUuidCard() {
    return this->_uuidCard;
}

void Card::setUuidCard(String uuidCard) {
    this->_uuidCard = uuidCard;
}

String Card::getUuidToken() {
    return this->_uuidToken;
}

void Card::setUuidToken(String uuidToken) {
    this->_uuidToken = uuidToken;
}

String Card::getToken() {
    return this->_token;
}

void Card::setToken(String token) {
    this->_token = token;
}