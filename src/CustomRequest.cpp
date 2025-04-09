
#include <string.h> // Necesario para strcmp
#include <CustomRequests.h>
#if TARGET_PLATFORM == 0
    
    #include <SPI.h>

    CustomRequests::CustomRequests(IPAddress arduinoIP, IPAddress proxyIP)
    : _arduinoIP(arduinoIP), _proxyIP(proxyIP)
    {

        Serial.println(F("Leyendo info del lector NFC"));
        Ethernet.init(pinETH);
        /*
        if (dataStore.activateSD()) {
            _currentDeviceInfo = dataStore.getDeviceInfo();
        }
        dataStore.deactivateSD();
        */

        SharedKey sharedKey("", "");
        _currentDeviceInfo = DeviceInfo("nfc-reader", "demond", "1234567", "", sharedKey);

        Serial.println(F("Esta es la información del lector"));
        Serial.println(_currentDeviceInfo.toString());
    }

    bool CustomRequests::_connectToProxy()
    {
        Serial.println("Iniciando Ethernet");
        delay(10);
        
        Ethernet.begin(_mac, _arduinoIP);

        Serial.print(F("Dirección IP del Arduino: "));
        Serial.println(Ethernet.localIP());

        return _client.connect(_proxyIP, _proxyPort) == 1;
    }

#else 
    CustomRequests::CustomRequests(const char * ssid, const char * pass) : _ssid(ssid), _pass(pass) {
        SharedKey sharedKey("", "");
        _currentDeviceInfo = DeviceInfo("nfc-reader", "demond", "1234567", "", sharedKey);
    
        Serial.println(F("Esta es la información del lector"));
        Serial.println(_currentDeviceInfo.toString());
    }

    bool CustomRequests::_connectToWiFi() {
        int count = 0;
        const int retries = 10;
        if(WiFi.status() != WL_CONNECTED){
            WiFi.begin(_ssid, _pass);
            Serial.println(F("Conectando a"));
            Serial.println(_ssid);

            while (WiFi.status() != WL_CONNECTED && count <= retries) {
                delay(200);
                Serial.print(F("."));
                count++;
            }
            
            if(count > retries)
                return false;

            return true;
        }
        else {
            return true;
        }
    }

#endif

bool CustomRequests::_connect(){
    #if TARGET_PLATFORM == 0
        return _connectToProxy();
    #else
        return _connectToWiFi();
    #endif
}

JsonDocument CustomRequests::_parseJson()
{
    #if TARGET_PLATFORM == 0
        char status[32] = {0};
        _client.readBytesUntil('\r', status, sizeof(status));

        if (strcmp(status + 9, "200 OK") == 0 || strcmp(status + 9, "204 No Content") == 0 || strcmp(status + 9, "401 Unauthorized") == 0)
        {
            if (!_client.find("\r\n\r\n"))
            {
                Serial.println(F("Invalid response"));
                _client.stop();
                return JsonDocument();
            }

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, _client);
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                _client.stop();
                return JsonDocument();
            }
            return doc;
        }
        else
        {
            Serial.print(F("Unexpected response: "));
            Serial.println(status);
            _client.stop();
            return JsonDocument();
        }
    #else
        if (_httpCode > 0) {
            Serial.printf("[HTTP] POST... code: %d\n", _httpCode);
        
            if (_httpCode == HTTP_CODE_OK || _httpCode == HTTP_CODE_CREATED || _httpCode == HTTP_CODE_NO_CONTENT || _httpCode == HTTP_CODE_UNAUTHORIZED) {
            String responsePayload = _http.getString();
            Serial.println("Cuerpo JSON de la respuesta POST:");
            Serial.println(responsePayload);
        
            // Parsear el JSON de la respuesta
            JsonDocument doc; // Ajusta el tamaño según la respuesta esperada
            DeserializationError error = deserializeJson(doc, responsePayload);
        
            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                _http.end();
                return JsonDocument();
            }
            return doc;

            } else {
            Serial.printf("[HTTP] POST... respuesta inesperada: %d\n", _httpCode);
            Serial.println("Cuerpo de la respuesta:");
            Serial.println(_http.getString()); // Imprimir el cuerpo para depuración
            }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", _http.errorToString(_httpCode).c_str());
        _http.end();
        return JsonDocument();
      }
    #endif
}

void CustomRequests::_prepareQuery(const char *method, const char *uri, const char *contentType, bool auth, const char *token, const JsonDocument &docRequest)
{
    #if TARGET_PLATFORM == 0
        _client.print(method);
        _client.print(F(" "));
        _client.print(uri);
        _client.print(F(" HTTP/1.1\r\n"));
        _client.print(F("Host: "));
        _client.print(_HOST);
        _client.print(F("\r\nContent-Type: "));
        _client.print(contentType);
        _client.print(F("\r\n"));

        if (auth)
        {
            _client.print(F("Authorization: Bearer "));
            _client.print(token);
            _client.print(F("\r\n"));
        }

        _client.print(F("Content-Length: "));
        _client.print(measureJsonPretty(docRequest));
        _client.print(F("\r\nConnection: close \r\n\r\n"));
        serializeJsonPretty(docRequest, _client);
    #else
        _http.begin(String(_HOST)+String(uri));
        _http.addHeader(F("Content-Type"), contentType);
        String json;
        serializeJsonPretty(docRequest, json);
        _httpCode = _http.POST(json);
    #endif

}

const char *CustomRequests::registerCard()
{

    Serial.println(F("Requesting card uuid"));

    if (_currentDeviceInfo.getUserName() != nullptr && _currentDeviceInfo.getUserName()[0] != '\0' &&
        _currentDeviceInfo.getPass() != nullptr && _currentDeviceInfo.getPass()[0] != '\0' &&
        _currentDeviceInfo.getUuid() != nullptr && _currentDeviceInfo.getUuid()[0] != '\0')
    {
        if (_connect())
        {
            JsonDocument docRequest;
            docRequest["nfcUuid"] = _currentDeviceInfo.getUuid();
            _prepareQuery("POST", "/api/cards", "application/json", true, _currentDeviceInfo.getToken(), docRequest);

            Serial.println(F("Solicitud enviada"));

            JsonDocument doc = _parseJson();
            _disconnect();
            if (doc["error"].is<JsonObject>())
            {
                JsonObject errorObj = doc["error"];
                int status = errorObj["status"];
                if (status == 401)
                {
                    Serial.println(F("Error de autorización"));
                    _authNFC();
                    if (_currentDeviceInfo.getUserName() != nullptr && _currentDeviceInfo.getUserName()[0] != '\0' &&
                        _currentDeviceInfo.getPass() != nullptr && _currentDeviceInfo.getPass()[0] != '\0')
                    {
                        return registerCard();
                    }
                    return "";
                }
                return "";
            }

            const char *uuidCard = const_cast<char *>(doc["card"]["uuid"].as<const char *>());
            Serial.println(F("End requesting card uuid"));
            return uuidCard;
        }
    }
    return "";
}

void CustomRequests::_authNFC()
{
    Serial.println(F("Requesting token NFC"));

    if (_connect())
    {
        JsonDocument docRequest;
        docRequest["userName"] = _currentDeviceInfo.getUserName();
        docRequest["pass"] = _currentDeviceInfo.getPass();
        _prepareQuery("POST", "/api/nfc-readers/auth", "application/json", false, "", docRequest);

        Serial.println(F("Solicitud enviada"));

        JsonDocument doc = _parseJson();
        _disconnect();

        if (!doc.isNull())
        {
            SharedKey sharedKey(doc["sharedKeyUuid"].as<const char *>(), doc["sharedKey"].as<const char *>());
            _currentDeviceInfo.setSharedKey(sharedKey);
            _currentDeviceInfo.setPass(_currentDeviceInfo.getPass());
            _currentDeviceInfo.setUserName(_currentDeviceInfo.getUserName());
            _currentDeviceInfo.setUuid(doc["NFCReaderUuid"].as<const char *>());
            _currentDeviceInfo.setToken(doc["token"].as<const char *>());

            Serial.println(F("Este es el token"));
            Serial.println(_currentDeviceInfo.getToken());
            /*
            if ( _dataStore.activateSD()) {
                _dataStore.storeDeviceInfo(_currentDeviceInfo);
                _dataStore.deactivateSD();
            }
            */

            Serial.println(F("End requesting token"));
        }
    }
}

Card CustomRequests::generateToken(Card &card)
{
    Serial.println(F("Requesting token"));

    if (_currentDeviceInfo.getUserName() != nullptr && _currentDeviceInfo.getUserName()[0] != '\0' &&
        _currentDeviceInfo.getPass() != nullptr && _currentDeviceInfo.getPass()[0] != '\0' &&
        _currentDeviceInfo.getUuid() != nullptr && _currentDeviceInfo.getUuid()[0] != '\0' &&
        card.getUuidCard() != nullptr && card.getUuidCard()[0] != '\0')
    {
        if (_connect())
        {
            JsonDocument docRequest;
            docRequest["cardUuid"] = card.getUuidCard();
            docRequest["nfcUuid"] = _currentDeviceInfo.getUuid();
            _prepareQuery("POST", "/api/cards/generate", "application/json", true, _currentDeviceInfo.getToken(), docRequest);

            Serial.println(F("Solicitud enviada"));

            JsonDocument doc = _parseJson();

            if (doc["error"].is<JsonObject>())
            {
                JsonObject errorObj = doc["error"];
                int status = errorObj["status"];
                if (status == 401)
                {
                    Serial.println(F("Error de autorización"));
                    _authNFC();
                    if (_currentDeviceInfo.getUserName() != nullptr && _currentDeviceInfo.getUserName()[0] != '\0' &&
                        _currentDeviceInfo.getPass() != nullptr && _currentDeviceInfo.getPass()[0] != '\0')
                    {
                        return generateToken(card);
                    }
                    return Card("", "", "");
                }
            }

            _disconnect();

            JsonArray tokenArray = doc["token"].as<JsonArray>();
            char token[513] = ""; // Buffer para almacenar el token (tamaño 512)
            int tokenIndex = 0;   // Índice para llevar el seguimiento de la posición en el buffer

            Serial.println(F("TokenJson"));
            for (JsonVariant v : tokenArray)
            {
                const char *tokenAux = v.as<const char *>(); // Obtén el valor como const char*
                Serial.println(tokenAux);

                // Asegúrate de no desbordar el buffer
                if (tokenIndex + strlen(tokenAux) < sizeof(token) - 1)
                {
                    strcpy(token + tokenIndex, tokenAux); // Copia la cadena al buffer
                    tokenIndex += strlen(tokenAux);       // Actualiza el índice
                }
                else
                {
                    Serial.println(F("¡Advertencia! El token excede el tamaño del buffer."));
                    // Puedes implementar un manejo de error aquí si es necesario
                    break; // Detener la concatenación para evitar desbordamiento
                }
            }
            token[tokenIndex] = '\0';

            char *uuidTokensVersion = const_cast<char *>(doc["tokensVersionUuid"].as<const char *>());

            Serial.println(F("TokensVersionUuid"));
            Serial.println(uuidTokensVersion);

            card.setUuidToken(uuidTokensVersion);
            card.setToken(token);

            Serial.println(F("Si construye la tarjeta"));
            return card;
        }
    }
    return Card("", "", "");
}

void CustomRequests::_disconnect()
{
    #if TARGET_PLATFORM == 0
        _client.stop();
        //digitalWrite(pinETH, HIGH);
        delay(10);
    #else
        _http.end();
    #endif
    
}

const DeviceInfo &CustomRequests::getCurrentDeviceInfo()
{
    return _currentDeviceInfo;
}