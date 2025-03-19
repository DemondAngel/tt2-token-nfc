#include <Card.h>

Card::Card(String uuid_card, String uuid_token, String token){
    this->_uuid_card = uuid_card;
    this->_uuid_token = uuid_token;
    this->_token = token;
}

String Card::getUuidCard(){
    return this->_uuid_card;
}

void Card::setUuidCard(String uuid_card){
    this->_uuid_card = uuid_card;
}

String Card::getUuidToken(){
    return this->_uuid_token;
}

void Card::setUuidToken(String uuid_token){
    this->_uuid_token = uuid_token;
}

String Card::getToken(){
    return this->_token;
}

void Card::setToken(String token){
    this->_token = token;
}