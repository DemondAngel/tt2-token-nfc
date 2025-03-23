#include <StoreData.h>
#include <SPI.h>
#include <SD.h>

const int pinCS_SD = 4;

char * deviceInfoFile = "Info.txt";

JsonDocument parseJsonFromFile(char * fileName){
    Serial.println("Reading file");
    Serial.println(fileName);

    File file = SD.open(fileName, FILE_READ);

    if(!file) {
        Serial.println("There was an error opening the file");
        return JsonDocument();
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if(error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return JsonDocument();
    }

    return doc;
}

void parseJsonToFile(JsonDocument doc, char * fileName){
    SD.remove(fileName);
    File file = SD.open(fileName, FILE_WRITE);

    if(!file) {
        Serial.println("There was an erro opening the file");
        return;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Error al escribir en el archivo.");
    } else {
        Serial.println("Datos guardados en el archivo JSON.");
    }

    file.close();
}

bool activateSD(){

    digitalWrite(pinCS_SD, LOW);
    return SD.begin(pinCS_SD);
}

void deactivateSD(){
    SD.end();
    digitalWrite(pinCS_SD, HIGH);
}

DeviceInfo getDeviceInfo(){
    
    JsonDocument doc = parseJsonFromFile(deviceInfoFile);

    if(doc.isNull()){
        Serial.println("Hubo un error convirtiendo el doc");
        SharedKey sharedKey("", "");
        return DeviceInfo("", "", "", "", sharedKey);
    }
    
    SharedKey sharedKey(doc["sharedKey"]["uuid"], doc["sharedKey"]["sharedKey"]);

    DeviceInfo deviceInfo(doc["uuid"], doc["userName"], doc["pass"], doc["token"], sharedKey);
    Serial.println("Este es el userName");
    Serial.println(deviceInfo.getUserName());
    return deviceInfo;
}



void storeDeviceInfo(DeviceInfo deviceInfo){
    
    JsonDocument doc;
    JsonDocument docSharedKey;

    docSharedKey["uuid"] = deviceInfo.getSharedKey().getUuidSharedKey();
    docSharedKey["sharedKey"] = deviceInfo.getSharedKey().getSharedKey();

    doc["uuid"] = deviceInfo.getUuid();
    doc["userName"] = deviceInfo.getUserName();
    doc["pass"] = deviceInfo.getPass();
    doc["token"] = deviceInfo.getToken();
    doc["sharedKey"] = docSharedKey;

    parseJsonToFile(doc, deviceInfoFile);

}