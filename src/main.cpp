#include <LiquidCrystal.h>
#include <ReaderWriterNFC.h>
//#include <CipherNFC.h>
#include <Arduino.h>
#include <CustomRequests.h>
#include <StoreData.h>

int regis = 0;

LiquidCrystal lcd(8, 9, 14, 5, 6, 7);  // Pin definition of the LCD

void setup() {
  Serial.begin(115200);

  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(53, OUTPUT);
  
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("Inicializando NFC");
  initPN532();

  Serial.begin(115200);
  Serial.println("Inicializando Ethernet...");

}

void loop() {

  regis = digitalRead(3);

  if(regis == HIGH){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Registro activo.");
    lcd.setCursor(0,1);
    lcd.print("Acerca tarjeta.");
    Serial.println("Autenticando NFC");
    if(detectCard()){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Registrando...");
      
      const char* uuidCard = registerCard();
      if(uuidCard != NULL){
        Serial.println("Esta es el uuId nuevo de la tarjeta");
        Serial.println(uuidCard);

        Card card(uuidCard, "", "");

        card = generateToken(card);

        Serial.println("ESte es el token de la tarjeta");
        Serial.println(card.getToken());

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tarjeta");
        lcd.setCursor(0,1);
        lcd.print("registrada");
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Hubo un error :()");
      }
      
    }

  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Transaccion");
    lcd.setCursor(0,1);
    lcd.print("Acerque tarjeta");
  }

  delay(2000);
}