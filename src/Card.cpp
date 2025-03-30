#include <Card.h>
#include <string.h>
#include <Arduino.h> // Para Serial (opcional, para mensajes de advertencia)

Card::Card() {
    _uuidCard[0] = '\0';
    _uuidToken[0] = '\0';
    _token[0] = '\0';
}

Card::Card(const char* uuidCard, const char* uuidToken, const char* token) {
    setUuidCard(uuidCard);
    setUuidToken(uuidToken);
    setToken(token);
}

void Card::setUuidCard(const char* uuidCard) {
    if (uuidCard != nullptr) {
        strncpy(_uuidCard, uuidCard, CARD_UUID_LENGTH - 1);
        _uuidCard[CARD_UUID_LENGTH - 1] = '\0';
    } else {
        _uuidCard[0] = '\0';
    }
}

const char* Card::getUuidCard() const {
    return _uuidCard;
}

void Card::setUuidToken(const char* uuidToken) {
    if (uuidToken != nullptr) {
        strncpy(_uuidToken, uuidToken, TOKEN_UUID_LENGTH - 1);
        _uuidToken[TOKEN_UUID_LENGTH - 1] = '\0';
    } else {
        _uuidToken[0] = '\0';
    }
}

const char* Card::getUuidToken() const {
    return _uuidToken;
}

bool Card::setToken(const char* token) {
    if (token != nullptr) {
        size_t len = strlen(token);
        if (len < CARD_TOKEN_LENGTH) {
            strncpy(_token, token, CARD_TOKEN_LENGTH - 1);
            _token[CARD_TOKEN_LENGTH - 1] = '\0';
            return true; // Éxito
        } else {
            // Opción: Truncar la cadena (sin indicar error)
            strncpy(_token, token, CARD_TOKEN_LENGTH - 1);
            _token[CARD_TOKEN_LENGTH - 1] = '\0';
            // Opcional: Mostrar una advertencia (si es útil para depuración)
            // Serial.println("Advertencia: Token demasiado largo, truncado.");
            return false; // Fallo (se truncó)
        }
    } else {
        _token[0] = '\0';
        return true; // Considerar éxito al establecer una cadena vacía
    }
}

const char* Card::getToken() const {
    return _token;
}