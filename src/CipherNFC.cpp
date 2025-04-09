#include <CipherNFC.h>
#include <AES.h>
#include <string.h>

EncryptedMessage::EncryptedMessage() : _len(0)
{
    memset(_data, 0, sizeof(_data));
}

EncryptedMessage::EncryptedMessage(const uint8_t *data, size_t len) : _len(0)
{
    memset(_data, 0, sizeof(_data));
    setData(data, len);
}

void EncryptedMessage::setData(const uint8_t *data, size_t len)
{
    if (len > sizeof(_data))
    {
        len = sizeof(_data);
        // Consider adding a warning here if the data is truncated
    }
    memcpy(_data, data, len);
    _len = len;
}

 uint8_t *EncryptedMessage::getData()
{
    return _data;
}

void EncryptedMessage::setLen(size_t len)
{
    if (len > sizeof(_data))
    {
        _len = sizeof(_data);
        // Consider adding a warning here if the length is too large
    }
    else
    {
        _len = len;
    }
}

const size_t EncryptedMessage::getLen()
{
    return _len;
}

// Implementación de KeysInformation
KeysInformation::KeysInformation()
{
    memset(aesKey, 0, sizeof(aesKey));
}

KeysInformation::KeysInformation(const SharedKey &sharedKey)
{
    /*
    if(deviceInfo.getSharedKey().getSharedKey() != "" && deviceInfo.getSharedKey().getIv() != ""){

        String sharedKey = deviceInfo.getSharedKey().getSharedKey();
        String ivKey = deviceInfo.getSharedKey().getIv();
        int sharedKeySize = sharedKey.length();
        int ivKeySize = ivKey.length();

        keysInformation = KeysInformation(sharedKeySize, ivKeySize);

        for(int i = 0; i < sharedKeySize; i++){
            keysInformation.aesKey[i] = (uint8_t) sharedKey[i];
            keysInformation.aesIv[i] = (uint8_t) ivKey[i];
        }

    }*/

    memset(aesKey, 0, sizeof(aesKey));
    setAesKey(reinterpret_cast<const uint8_t *>(sharedKey.getSharedKey()));
}

void KeysInformation::setAesKey(const uint8_t *key)
{
    if (key != nullptr)
    {
        size_t len = strlen(reinterpret_cast<const char *>(key));
        if (len >= sizeof(aesKey))
        {
            len = sizeof(aesKey) - 1; // Ensure null termination
            // Consider adding a warning here if the key is truncated
        }
        memcpy(aesKey, key, len);
        aesKey[len] = '\0'; // Ensure null termination for string operations if needed
    }
    else
    {
        memset(aesKey, 0, sizeof(aesKey));
    }
}

const uint8_t *KeysInformation::getAesKey()
{
    return aesKey;
}

void EncryptionNFC::printHex(const char *label, const uint8_t *data, const size_t len)
{
    Serial.print(label);
    for (size_t i = 0; i < len; i++)
    {
        if (data[i] < 0x10)
        {
            Serial.print("0");
        }
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

EncryptionNFC::EncryptionNFC() {}

EncryptionNFC::EncryptionNFC(const KeysInformation &keysInformation, const EncryptedMessage &encryptedMessage)
    : _keysInformation(keysInformation), _encryptedMessage(encryptedMessage) {}

void EncryptionNFC::encryptNFCData(const char *message)
{
    if (_keysInformation.getAesKey() != nullptr && _keysInformation.getAesKey()[0] != '\0')
    {
        Serial.println(F("Este es el mensaje a cifrar"));
        Serial.println(message);

        size_t messageLength = strlen(message);
        size_t paddedLength = ((messageLength / 16) + 1) * 16;

        Serial.println("paddedLength");
        Serial.println(paddedLength);

        printHex("Llave de cifrado", _keysInformation.getAesKey(), 16);

        _encryptedMessage.setLen(paddedLength);
        uint8_t messageArray[513];

        memcpy(messageArray, message, messageLength);
        uint8_t paddingValue = paddedLength - messageLength;
        for (size_t i = messageLength; i < paddedLength; i++)
        {
            messageArray[i] = paddingValue;
        }

        printHex("Mensaje plano: ", messageArray, paddedLength);

        AES128 aes128;
        aes128.setKey(_keysInformation.getAesKey(), 16);

        uint8_t outputData[513];

        for (size_t i = 0; i < paddedLength; i += 16) {
            aes128.encryptBlock(&outputData[i], &messageArray[i]);
        }

        _encryptedMessage.setData(outputData, paddedLength);

        printHex("Mensaje cifrado: ", outputData, _encryptedMessage.getLen());
        Serial.println(F("Longitud del mensaje cifrado"));
        Serial.println(_encryptedMessage.getLen());
    }
    else
    {
        Serial.println(F("Error: Clave AES o IV no válidos (vacíos)."));
    }
}

const char *EncryptionNFC::decryptNFCData()
{
    // La función de descifrado no necesita cambios en este aspecto,
    // ya que trabaja con los datos cifrados que ya están en _encryptedMessage.
    if (_keysInformation.getAesKey() != nullptr && _keysInformation.getAesKey()[0] != '\0')
    {

        Serial.println(F("Inicializando descifrado"));

        size_t encryptedLength = _encryptedMessage.getLen();
        if (encryptedLength == 0)
        {
            Serial.println(F("Advertencia: No hay datos para descifrar."));
            return nullptr;
        }

        AES128 aes128;
        const size_t keySize = 16;
        aes128.setKey(_keysInformation.getAesKey(),keySize);
        printHex("Llave de descifrado", _keysInformation.getAesKey(), keySize);
        uint8_t decrypted[513];
        uint8_t* encryptedData = _encryptedMessage.getData();

        for (size_t i = 0; i < encryptedLength; i += 16) {
            aes128.decryptBlock(&decrypted[i], &encryptedData[i]);
        }

        if (decrypted != nullptr)
        {
            uint8_t paddingValue = decrypted[encryptedLength - 1];
            printHex("Mensaje a descifrado: ", decrypted, _encryptedMessage.getLen());
            if (paddingValue > 0)
            {
                size_t realLength = encryptedLength - paddingValue;
                Serial.print(F("Real Length: "));
                Serial.println(realLength);
                Serial.println(F("Convirtiendo"));
                char result[realLength+1];
                if (result == nullptr)
                {
                    Serial.println(F("Error al asignar memoria para el resultado del descifrado."));
                    return nullptr;
                }
                memcpy(result, decrypted, realLength);
                result[realLength] = '\0';
                Serial.println(result);
                return result;
            }
            else
            {
                Serial.println(F("Error: Padding PKCS7 inválido."));
                return nullptr;
            }
        }
        else
        {
            Serial.println(F("Error durante el descifrado con AESLib."));
            return nullptr;
        }
    }
    else
    {
        Serial.println(F("Error: Clave AES o IV no válidos (vacíos)."));
        return nullptr;
    }
}

EncryptedMessage EncryptionNFC::getEncryptedMessage()
{
    return _encryptedMessage;
}