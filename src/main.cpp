#include <LiquidCrystal.h>
#include <ReaderWriterNFC.h>
//#include <CipherNFC.h>
#include <Arduino.h>
#include <CustomRequests.h>

int regis = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // Pin definition of the LCD

void setup() {
  Serial.begin(115200);

  pinMode(3, INPUT);
  
  /*lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("Inicializando NFC");
  initPN532();*/

  Serial.begin(115200);
  Serial.println("Inicializando Ethernet...");

}

void loop() {

  regis = digitalRead(3);

  if(regis == HIGH){
    Serial.println("Autenticando NFC");
    String token = authNFC("Demond", ">Demond4567");
  }

  delay(2000);
}