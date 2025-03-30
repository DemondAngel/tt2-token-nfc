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
  #define D4 14
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
  const char * ssd = (const char *) F("User");
  const char * pass = (const char *) F("PASS");
#endif

#include <CustomRequests.h>
#include <ReaderWriterNFC.h>

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // Pin definition of the LCD

void setup() {
  Serial.begin(115200);

  pinMode(REGISTER_PIN, INPUT);
  //pinMode(pinETH, OUTPUT);

  lcd.begin(COLS, ROWS);
  lcd.setCursor(0, 0);
  lcd.print(F("Iniciando NFC"));
  Serial.println(F("Inicializando NFC"));
  initPN532();

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
    
    if (detectCard()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Registrando..."));

      #if TARGET_PLATFORM == 0
        CustomRequests request(arduinoIp, proxyIp);
      #else
        CustomRequests request("", "");
      #endif
      
      
      const char * uuidCard = request.registerCard();
      if (uuidCard != nullptr && uuidCard[0] != '\0') { // Comprobación para char * vacío
        Serial.println(F("Esta es el uuId nuevo de la tarjeta"));
        Serial.println(uuidCard);

        Card card;
        card.setUuidCard(uuidCard);

        card = request.generateToken(card);

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

          const char * decryptedMessage = encryptionNFC.decryptNFCData();

          Serial.println("Este es el mensaje descifrado");
          Serial.println(decryptedMessage);
          /*
          if (encryptedMessage.length != 0) {
            Serial.println(F("Descifrando"));
            String dataDecripted = decryptNFCData(deviceInfo);


            if (dataDecripted != "") {
              Serial.println(dataDecripted);

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print(F("Tarjeta"));
              lcd.setCursor(0, 1);
              lcd.print(F("Registrada"));
            }


          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(F("Error, verifica"));
            lcd.setCursor(0, 1);
            lcd.print(F("Encriptacion"));
          }*/
        
        } else {
          Serial.println(F("Error en recuperar la tarjeta"));
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(F("Error, verifica"));
          lcd.setCursor(0, 1);
          lcd.print(F("Conn y SD card"));
        }
        
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
  }

  delay(2000);
}