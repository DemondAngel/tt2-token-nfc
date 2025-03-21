#include <CustomRequests.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress arduinoIP(192, 168, 1, 100);
IPAddress proxyIP(192, 168, 1, 10);
const int proxyPort = 8080;
EthernetClient client;

JsonDocument parseJson() {

    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      client.stop();
      return JsonDocument();
    }

    if (strcmp(status + 9, "204 OK") != 0) {
        JsonDocument emptyDoc;
        return emptyDoc;
      }
  
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

String retrieveResponse(){
    bool jsonStart = false;
    String response = "";

    while (client.connected()) {
        while (client.available()) {
            char c = client.read();

            if (jsonStart) {
                response += c;
            } else {
                if (response.endsWith("\r\n\r\n")) {
                    jsonStart = true;
                    response = "{";  // Limpiar para solo almacenar JSON
                } else {
                    response += c;
                }
            }
        }
    }

    if (response.length() > 0) {
        Serial.println("Respuesta JSON recibida:");
        Serial.println(response);
    }

    return response;

}

bool connectToProxy(){
    Ethernet.begin(mac, arduinoIP);

    Serial.print("Dirección IP del Arduino: ");
    Serial.println(Ethernet.localIP());

    return client.connect(proxyIP, proxyPort) == 1;
}

String retrieveToken(){
    return "Token"; //Programación pendiente hasta que tenga SD
}

void storeToken(String token){
//Programación pendiente hasta que tenga SD
}

const char * registerCard(){
    const String uuid_nfc = "nfc-reader1";
    String jsonPayload = "{\"uuid_nfc\": \""+uuid_nfc+"\"}";
    client.print("POST /api/cards HTTP/1.1\r\n");
    client.print("Host: 8080-idx-tt2-token-protection-1740550210828.cluster-wfwbjypkvnfkaqiqzlu3ikwjhe.cloudworkstations.dev\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuZmMtcmVhZGVyIjp7InV1aWQiOiJuZmMtcmVhZGVyMSJ9LCJpYXQiOjE3NDI1MzY5MDAsImV4cCI6MTc0NTEyODkwMH0.mU2m5ZkK02p0PZueB2mhuVd9RRV0VXYgFXW7bTjrlIA\r\n");
    client.print("Content-Length: ");
    client.print(jsonPayload.length());
    client.print("\r\n");
    client.print("Connection: close \r\n\r\n");
    client.print(jsonPayload);

    Serial.println("Solicitud enviada");

    JsonDocument doc = parseJson();

    const char * uuidCard = doc["card"]["uuid"].as<const char*>();

    return uuidCard;
}

const char * authNFC(String username, String pass){
    String jsonPayload = "{\"username\": \""+username+"\",\"pass\": \""+pass+"\"}";
    client.print("POST /api/nfc-readers/auth HTTP/1.1\r\n");
    client.print("Host: 8080-idx-tt2-token-protection-1740550210828.cluster-wfwbjypkvnfkaqiqzlu3ikwjhe.cloudworkstations.dev\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(jsonPayload.length());
    client.print("\r\n");
    client.print("Connection: close \r\n\r\n");
    client.print(jsonPayload);

    Serial.println("Solicitud enviada");

    JsonDocument doc = parseJson();

    const char * token = doc["token"].as<const char*>();

    return token;
}

Card generateToken(Card card, String uuid_nfc){
    String jsonPayload = "{\"uuid_card\": \""+card.getUuidCard()+"\",\"uuid_nfc\": \""+uuid_nfc+"\"}";
    client.print("POST /api/cards/generate HTTP/1.1\r\n");
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

    String token = doc["token"].as<String>();
    String uuidTokensVersion = doc["uuid_tokens_version"].as<String>();

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
}