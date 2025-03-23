#include <CustomRequests.h>
#include <SPI.h>
#include <Ethernet.h>
#include <StoreData.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress arduinoIP(192, 168, 1, 100);
IPAddress proxyIP(192, 168, 1, 10);
const int pin_ETH = 53;
const int proxyPort = 8080;
String host = "8080-idx-tt2-token-protection-1740550210828.cluster-wfwbjypkvnfkaqiqzlu3ikwjhe.cloudworkstations.dev";
EthernetClient client;

JsonDocument parseJson() {

    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") == 0 || strcmp(status + 9, "204 No Content") == 0 || strcmp(status + 9, "401 Unauthorized") == 0) {
    // Skip HTTP headers
        char endOfHeaders[] = "\r\n\r\n";
        if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response"));
        client.stop();
        return JsonDocument();
        }

        JsonDocument doc;

        // Parse JSON object
        DeserializationError error = deserializeJson(doc,client);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            client.stop();
            return JsonDocument();
        }

        return doc;
    }
    else {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        client.stop();
        return JsonDocument();
    }
  
}

bool connectToProxy(){
    digitalWrite(pin_ETH, LOW);

    Ethernet.begin(mac, arduinoIP);

    Serial.print("Dirección IP del Arduino: ");
    Serial.println(Ethernet.localIP());

    return client.connect(proxyIP, proxyPort) == 1;
}

const char * registerCard(){
    Serial.println("Requesting card uuid");

    disconnect();
    if(activateSD()){
        Serial.println("Se ha activado el SD");
    }
    DeviceInfo deviceInfo = getDeviceInfo();
    deactivateSD();
    connectToProxy();

    String jsonPayload = "{\"nfcUuid\": \""+deviceInfo.getUuid()+"\"}";
    client.print("POST /api/cards HTTP/1.1\r\n");
    client.print("Host: "+host+"\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Authorization: Bearer " + deviceInfo.getToken()+"\r\n");
    client.print("Content-Length: ");
    client.print(jsonPayload.length());
    client.print("\r\n");
    client.print("Connection: close \r\n\r\n");
    client.print(jsonPayload);

    Serial.println("Solicitud enviada");

    JsonDocument doc = parseJson();

    if(doc.containsKey("error")){
        JsonObject errorObj = doc["error"];
        int status = errorObj["status"];

        // Manejar el error según el código de estado
        if (status == 401) {
            Serial.println("Error de autorización: ");
            deviceInfo = authNFC(deviceInfo.getUserName(), deviceInfo.getPass());

            return registerCard();
        }
    }
    disconnect();

    const char * uuidCard = doc["card"]["uuid"].as<const char*>();
    Serial.println("End requesting card uuid");
    return uuidCard;
}

DeviceInfo authNFC(String userName, String pass){
    Serial.println("Requesting token");
    connectToProxy();
    String jsonPayload = "{\"userName\": \""+userName+"\",\"pass\": \""+pass+"\"}";
    client.print("POST /api/nfc-readers/auth HTTP/1.1\r\n");
    client.print("Host: "+host+"\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(jsonPayload.length());
    client.print("\r\n");
    client.print("Connection: close \r\n\r\n");
    client.print(jsonPayload);

    Serial.println("Solicitud enviada");

    JsonDocument doc = parseJson();
    disconnect();

    SharedKey sharedKey(doc["sharedKeyUuid"], doc["sharedKey"]);
    DeviceInfo deviceInfo(doc["NFCReaderUuid"], userName, pass, doc["token"], sharedKey);
    Serial.println("Este es el token");
    Serial.println(deviceInfo.getToken());
    activateSD();
    storeDeviceInfo(deviceInfo);
    deactivateSD();

    Serial.println("End requesting token");
    return deviceInfo;
}

Card generateToken(Card card){

    Serial.println("Requesting token");

    disconnect();
    if(activateSD()){
        Serial.println("SD card activated");
    }

    DeviceInfo deviceInfo = getDeviceInfo();
    deactivateSD();
    connectToProxy();

    String jsonPayload = "{\"cardUuid\": \""+card.getUuidCard()+"\",\"nfcUuid\": \""+deviceInfo.getUuid()+"\"}";
    client.print("POST /api/cards/generate HTTP/1.1\r\n");
    client.print("Host: "+host+"\r\n");
    client.print("Authorization: Bearer "+deviceInfo.getToken()+"\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(jsonPayload.length());
    client.print("\r\n");
    client.print("Connection: close \r\n\r\n");
    client.print(jsonPayload);

    Serial.println("Solicitud enviada");

    JsonDocument doc = parseJson();

    if(doc.containsKey("error")){
        JsonObject errorObj = doc["error"];
        int status = errorObj["status"];

        // Manejar el error según el código de estado
        if (status == 401) {
            Serial.println("Error de autorización: ");
            deviceInfo = authNFC(deviceInfo.getUserName(), deviceInfo.getPass());

            return generateToken(card);
        }
    }

    disconnect();

    String token = doc["token"].as<String>();
    String uuidTokensVersion = doc["tokensVersionUuid"].as<String>();

    card = Card(card.getUuidCard(), uuidTokensVersion,token);

    return card;
}

boolean validateToken(Card card){
    String uuid_nfc = "nfc-reader-1";
    String jsonPayload = "{\"uuid_card\": \""+card.getUuidCard()+"\",\"uuid_nfc\": \""+uuid_nfc+"\"}";
    client.print("POST /api/cards/validate HTTP/1.1\r\n");
    client.print("Host: 8080-idx-tt2-token-protection-1740550210828.cluster-wfwbjypkvnfkaqiqzlu3ikwjhe.cloudworkstations.dev\r\n");
    client.print("Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuZmMtcmVhZGVyIjp7InV1aWQiOiJuZmMtcmVhZGVyMSJ9LCJpYXQiOjE3NDI1MzY5MDAsImV4cCI6MTc0NTEyODkwMH0.mU2m5ZkK02p0PZueB2mhuVd9RRV0VXYgFXW7bTjrlIA\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(jsonPayload.length());
    client.print("\r\n");
    client.print("Connection: close \r\n\r\n");
    client.print(jsonPayload);

    Serial.println("Solicitud enviada");

    JsonDocument doc = parseJson();

    if(!doc.isNull()){
        return true;
    }

    return false;
}

SharedKey getSharedKeyNFC(String uuid_shared_key){
    
}

void disconnect(){
    client.stop();
    digitalWrite(pin_ETH, HIGH);
    
}