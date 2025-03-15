#include <CipherNFC.h>
#include <uECC.h>
#include "aes.hpp"

/*Dummy data*/
uint8_t aesKey[16] = {0x01, 0xC3, 0x25, 0xB5, 0x3C, 0x4F, 0xD8, 0x8A, 0xC1, 0x5B, 0x22, 0xDD, 0xD4, 0xEE, 0x30, 0x5D};  // La clave generada a través de ECC
uint8_t aesIV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

/* Mensaje de ejemplo */
uint8_t encryptedMessage[512];
uint8_t decryptedMessage[512];

const struct uECC_Curve_t *curve = uECC_secp256r1();
// Claves para testing
uint8_t privateKey[32] = {
    0x9B, 0x1E, 0xA7, 0x60, 0x1E, 0x8D, 0xC5, 0xB2,
    0x3F, 0x47, 0x7C, 0x28, 0x4B, 0xE2, 0xA4, 0x58,
    0x22, 0x14, 0x0E, 0x89, 0x49, 0xD3, 0x6F, 0x27,
    0x61, 0x9A, 0x14, 0x92, 0xA3, 0xDC, 0x3D, 0xD4
  };

uint8_t publicKey[64] = {
    0x04, 0x96, 0x76, 0x98, 0x12, 0xA0, 0x32, 0x71,
    0x45, 0x8E, 0xB5, 0x36, 0x15, 0x3B, 0x65, 0x49,
    0x9E, 0xF4, 0x1C, 0x24, 0xC3, 0x71, 0xB9, 0x09,
    0x8F, 0x64, 0xC3, 0xD1, 0xC9, 0x31, 0x76, 0x56,
    0x7F, 0xBB, 0x5A, 0x91, 0x2F, 0x4F, 0xE2, 0x51,
    0xD7, 0x67, 0x71, 0x9A, 0x44, 0x3F, 0x93, 0xAD,
    0x82, 0x47, 0x23, 0x8E, 0x76, 0x6C, 0xA2, 0xD7,
    0x3B, 0x8A, 0x4C, 0x94, 0x38, 0xE1, 0x13, 0x9B
};

void printHex(const char *label, const uint8_t *data, size_t len) { //Testint purposes
    Serial.print(label);
    Serial.print(": ");
    for (size_t i = 0; i < len; i++) {
        if (data[i] < 16) Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

// Función de padding PKCS7
void addPadding(uint8_t *data, size_t &len, size_t blockSize) {
    // Calcular el número de bytes de relleno necesarios
    size_t padding = blockSize - (len % blockSize);
    for (size_t i = len; i < len + padding; i++) {
        data[i] = (uint8_t)padding; // Relleno con el número de bytes de padding
    }
    len += padding; // Actualizar la longitud de los datos
}

int removePadding(uint8_t *data, size_t &len) {
    int paddingLength = data[len - 1]; // Último byte indica cuántos bytes son padding
    Serial.println("Tamanio de padding");
    Serial.println(paddingLength);
    if (paddingLength > 16) return len;   // Previene errores si el padding es inválido
    return len - paddingLength;      
}

uint8_t * encryptNFCDATA(char * message){
    
    Serial.println("Este es el mensaje a cifrar");
    Serial.println(message);
    /*
    if (uECC_shared_secret(publicKey, privateKey, aesKey, curve)) {
        Serial.println("Clave AES generada con éxito.");
        printHex("Clave AES cifrada con ECC", aesKey, 16);
    } else {
        Serial.println("Error generando clave AES con ECC.");
        return NULL;
    }*/

    size_t messageLength = strlen(message);
    size_t paddedLength = messageLength;
    addPadding((uint8_t*)message, paddedLength, 16); 

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, aesKey, aesIV);
    memcpy(encryptedMessage, message, messageLength);
    AES_CBC_encrypt_buffer(&ctx, encryptedMessage, messageLength);
    printHex("Mensaje cifrado", encryptedMessage, messageLength);

    return encryptedMessage;
}

char * decryptNFCDATA(uint8_t * data){

    struct AES_ctx ctx;
    size_t lenData = strlen(data);
    Serial.println("Len data");
    Serial.println(lenData);
    AES_init_ctx_iv(&ctx, aesKey, aesIV);
    memcpy(decryptedMessage, encryptedMessage, lenData);
    AES_CBC_decrypt_buffer(&ctx, decryptedMessage, lenData);

    int realLength = removePadding(decryptedMessage, lenData);
    Serial.println("ReaLength");
    Serial.println(realLength);
    // Mostrar mensaje descifrado
    Serial.print("Texto descifrado: ");
    for (int i = 0; i < realLength; i++) {
        Serial.print((char)decryptedMessage[i]);
    }
    Serial.println();
    

    return NULL;
}