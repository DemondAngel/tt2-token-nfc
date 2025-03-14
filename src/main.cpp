#include <LiquidCrystal.h>
#include <ReaderWriterNFC.h>;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // Pin definition of the LCD

void setup(void) {
  Serial.begin(115200);
  lcd.begin(20, 4);
  lcd.println("Inicializando lector NFC...");

  initPN532();
}

void loop(void) {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Esperando una");
  lcd.setCursor(0, 1);
  lcd.print("tarjeta NFC...");
  
  read();
  delay(5000);

  const char message[] = "Hola NFC, soy Ian";
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

  delay(2000);
}