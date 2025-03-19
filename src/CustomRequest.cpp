#include <CustomRequests.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress arduinoIP(192, 168, 1, 100);
IPAddress proxyIP(192, 168, 1, 10);
const int proxyPort = 8080;
EthernetClient client;

StaticJsonDocument<500> parseJson(String jsonResponse){
    StaticJsonDocument<500> doc;

    // Parsear el JSON
    DeserializationError error = deserializeJson(doc, jsonResponse);

    // Si hay un error, imprimir y devolver un documento vacío
    if (error) {
        Serial.print("❌ Error al parsear JSON: ");
        Serial.println(error.f_str());
        return StaticJsonDocument<200>();
    }

    return doc;  // Retornar el objeto JSON
}

String retrieveResponse(){
    while (client.connected()) {
        while (client.available()) {
            char c = client.read();

            if (jsonStart) {
                response += c;
            } else {
                if (response.endsWith("\r\n\r\n")) {
                    jsonStart = true;
                    response = "";  // Limpiar para solo almacenar JSON
                } else {
                    response += c;
                }
            }
        }
    }

    if (response.length() > 0) {
        Serial.println("📥 Respuesta JSON recibida:");
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

String registerCard(){
    return "Card registered"
}

String authNFC(String username, String pass){
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

    String jsonString = retrieveResponse();

    StaticJsonDocument<500> doc = parseJson(jsonString);

    String token = doc["token"].as<String>();

    return token;
}

Card generateToken(Card card, String uuid_nfc){

}

boolean validateToken(Card card){
    return false;
}

SharedKey getSharedKeyNFC(String uuid_shared_key){
    
}

void disconnect(){
    client.stop();
}