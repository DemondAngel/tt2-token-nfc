#include <LiquidCrystal.h>
#include <Arduino.h>
#include <string.h> // Necesario para strcmp
#include <CustomRequests.h>
#include <CipherNFC.h>

#define PLATFORM_MEGA 0
#define PLATFORM_ESP32 1

#ifndef TARGET_PLATFORM
  #define TARGET_PLATFORM PLATFORM_ESP32
#endif

#if TARGET_PLATFORM == 0
  #include <Ethernet.h>
  #define REGISTER_PIN 3
  #define RS 8
  #define EN 9
  #define D4 4
  #define D5 5
  #define D6 6
  #define D7 7
  #define COLS 20
  #define ROWS 4
  IPAddress arduinoIp(192, 168, 1, 100); // Define la IP de tu Arduino
  IPAddress proxyIp(192, 168, 1, 10);

#else
  #include <WiFi.h>
  #define REGISTER_PIN 32
  #define RS 19
  #define EN 23
  #define D4 18
  #define D5 17
  #define D6 16
  #define D7 15
  #define COLS 16
  #define ROWS 2
  const char * ssd = (const char *) F("IZZI-68BC");
  const char * pass = (const char *) F("7NHggFEEeXNcKrddYA");
#endif

#include <ReaderWriterNFC.h>

void requestAndSaveNewToken(Card &card, CustomRequests &request);
void decryptTokenCard(char * tokenDecrypted, CustomRequests &request);

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // Pin definition of the LCD

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

ReaderWriter nfcReader(nfc);
SharedKey sharedKey("", "");
DeviceInfo currentDeviceInfo = DeviceInfo("nfc-reader", "demond", "1234567", "", sharedKey);



void setup() {
  Serial.begin(115200);

  pinMode(REGISTER_PIN, INPUT);

  lcd.begin(COLS, ROWS);
  lcd.setCursor(0, 0);
  lcd.print(F("Iniciando NFC"));
  Serial.println(F("Inicializando NFC"));
  nfcReader.initPN532();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Leyendo info del"));
  lcd.setCursor(0, 1);
  lcd.print(F("lector NFC"));
}

void loop() {

  if (digitalRead(REGISTER_PIN) == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Registro activo."));
    lcd.setCursor(0, 1);
    lcd.print(F("Acerca tarjeta."));
    Serial.println(F("Autenticando NFC"));
    
    if (nfcReader.detectCard()) {
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Registrando..."));
      Serial.println(F("Registrando"));
      #if TARGET_PLATFORM == 0
        CustomRequests request(arduinoIp, proxyIp, currentDeviceInfo);
      #else
        CustomRequests request(ssd,pass, currentDeviceInfo);
      #endif
      Card card;
      char * buffer = card.getUuidCard();
      request.registerCard(buffer);
      Serial.println(F("Esta es el uuId nuevo de la tarjeta"));
      Serial.println(buffer);
      
      if (card.getUuidCard() != nullptr && card.getUuidCard()[0] != '\0') { // Comprobación para char * vacío
        

        requestAndSaveNewToken(card, request);
        
      } else {
        Serial.println(F("Error registering card"));
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Error, verifica"));
        lcd.setCursor(0, 1);
        lcd.print(F("Con. y SD card"));
      }
      
    }

  } else {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Transaccion"));
    lcd.setCursor(0, 1);
    lcd.print(F("Acerque tarjeta"));
    
    if(nfcReader.detectCard()){
      char uuidCard[33];
      uuidCard[32] = '\0';
      char uuidTokensVersion[33];
      uuidTokensVersion[32] = '\0';

      char tokenDecrypted[513];

      for(int i = 0; i < 513; i++)
        tokenDecrypted[i] = '\0';
      
      nfcReader.readUuidCard(uuidCard);
      nfcReader.readUuidTokensVersion(uuidTokensVersion);

      #if TARGET_PLATFORM == 0
        CustomRequests request(arduinoIp, proxyIp, currentDeviceInfo);
      #else
        CustomRequests request(ssd,pass, currentDeviceInfo);
      #endif

      Serial.println(F("ESte es el UUID card de la tarjeta leida"));
      Serial.println(uuidCard);
      Serial.println(F("Este es el uuid de la version del token"));
      Serial.println(uuidTokensVersion);

      decryptTokenCard(tokenDecrypted, request);

      Serial.println(F("Este es el token descifrado de la tarjeta"));
      Serial.println(tokenDecrypted);

      Card card(uuidCard, uuidTokensVersion, tokenDecrypted);

      bool status = request.validateToken(card);

      if(status){
        Serial.println(F("Tarjeta autorizada"));
        delay(200);
        requestAndSaveNewToken(card, request);
      }
      else{
        Serial.println(F("No autorizado por cualquier razon"));
      }

    }
    
  }

  delay(2000);
}

void requestAndSaveNewToken(Card &card, CustomRequests &request){
  request.generateToken(card);

  if (card.getToken() != nullptr && card.getToken()[0] != '\0') { // Comprobación para char * vacío
    Serial.println(F("UuidCard ya final"));
    Serial.println(card.getUuidCard());

    Serial.println(F("UuidToken ya final"));
    Serial.println(card.getUuidToken());

    Serial.println(F("Token final"));
    Serial.println(card.getToken());
    
    KeysInformation keysInformation(request.getCurrentDeviceInfo().getSharedKey());
    EncryptedMessage encryptedMessage;

    EncryptionNFC encryptionNFC(keysInformation, encryptedMessage);

    encryptionNFC.encryptNFCData(card.getToken());

    if(nfcReader.writeUuidCard(card.getUuidCard()) == 0 && nfcReader.writeUuidTokensVersion(card.getUuidToken()) == 0
      && nfcReader.writeUuidSharedKey(request.getCurrentDeviceInfo().getSharedKey().getUuidSharedKey()) == 0 && nfcReader.writeToken(encryptionNFC.getEncryptedMessage().getData(), encryptionNFC.getEncryptedMessage().getLen()) == 0){
      Serial.println("Datos guardos");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Tarjeta"));
      lcd.setCursor(0,1);
      lcd.print(F("registrada"));    
    }
    
  
  } else {
    Serial.println(F("Error en recuperar la tarjeta"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Error, verifica"));
    lcd.setCursor(0, 1);
    lcd.print(F("Conn y SD card"));
  }
}

void decryptTokenCard(char * tokenDecrypted, CustomRequests &request){
  Serial.println("Comienza recuperación y descifrado de token");
  uint8_t token[513];
  size_t len;
  token[512] = (uint8_t)'\0';
  len = nfcReader.readToken(token);
  char uuidSharedKey[33];
  uuidSharedKey[32] = '\0';
  nfcReader.readUuidSharedKey(uuidSharedKey);
  
  SharedKey sharedKeyD;
  sharedKeyD.setUuidSharedKey(uuidSharedKey);
  request.getSharedKeyNFC(sharedKeyD);

  KeysInformation keysInformation(sharedKeyD);
  EncryptedMessage encryptedMessage(token, len);
  EncryptionNFC encryptionNFC(keysInformation, encryptedMessage);

  encryptionNFC.decryptNFCData(tokenDecrypted);
}