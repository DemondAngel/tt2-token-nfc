#include <LiquidCrystal.h>
#include <ReaderWriterNFC.h>
#include <CipherNFC.h>
//#include <Arduino.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // Pin definition of the LCD

void setup() {
  Serial.begin(115200);
  
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("Inicializando NFC");
  initPN532();
}

void loop() {

  char message[] = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOjEyMywicm9sZSI6ImFkbWluIiwiaWF0IjoxNzAwMDAwMDAwLCJleHAiOjE3MDAwMzYwMDB9.QO4D2rKg_CYw3Hzp9ZqfXnPjU5FuVV9uMaZXuVsM7qE";
  Serial.println("Si esta chambeando");
  uint8_t * encryptedMessage = encryptNFCDATA(message);

  decryptNFCDATA(encryptedMessage);
  /*
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Esperando una");
  lcd.setCursor(0, 1);
  lcd.print("tarjeta NFC...");
  
  read();
  delay(5000);

  int response = write(message);

  if (response == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Se leyo NFC");
    delay(2000);
  } else if (response == 10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Texto muy largo");
  }
    */

  delay(2000);
}