#include <ReaderWriterNFC.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <math.h>

#if TARGET_PLATFORM == 0
  #define SDA_PIN 20
  #define SCL_PIN 21

#else
  #define SDA_PIN 21
  #define SCL_PIN 22
#endif

uint8_t key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void initPN532(){
    Wire.begin();
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("No se encontro el PN532");
        while (1)
        ;  // Detiene la ejecución
    }

    nfc.SAMConfig();
}

int writeBlock(int block, uint8_t* dataBytes, uint8_t* uid, uint8_t uidLength) {

    // Intentamos autenticar usando la clave A
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 1, key)) {
      //Serial.println("Autenticación exitosa.");
  
      // Escribir datos en el bloque
      if (nfc.mifareclassic_WriteDataBlock(block, dataBytes)) {
        //Serial.println("Datos escritos correctamente.");
      } else {
        //Serial.println("Error al escribir en el bloque.");
        return 1;
      }
    } else {
      //Serial.println("Error de autenticación.");
      return 2;
    }
  
    return 0;
  }
  
  uint8_t * readBlock(int block, uint8_t * uid, uint8_t uidLength) {
    static uint8_t data[16];
  
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 1, key)) {
      //Serial.println("Autenticación exitosa.");
  
      int success = nfc.mifareclassic_ReadDataBlock(block, data);  // Leer Bloque 1
      if (success) {
        //Serial.println("Datos leídos del Bloque :");
        //Serial.println(data);
  
      } else {
        //Serial.println("Error al leer el Bloque 0.");
      }
    } else {
      //Serial.println("Error de autenticación.");
    }
  
    return data;
  
  }
  
  int write(const char* mensaje) {
  
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
  
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
      int len = strlen(mensaje);
  
      int idxChar = 0;
  
      //Serial.println("Longitud del mensaje");
      //Serial.println(len);
  
      int blocksToWrite = (int)ceil(len / 16.0);
      //Serial.println("Cantidad de bloques a escribir");
      //Serial.println(blocksToWrite);
      int finalSector = (int)ceil(blocksToWrite / 2.0);
  
      if (finalSector > 15) {
        return 10;
      }
  
      uint8_t data[16] = { 0 };  // Inicializar el arreglo de datos con ceros
  
      // Convertir el entero a bytes y guardarlo en el arreglo
      data[0] = (len >> 8) & 0xFF;  // Primer byte
      data[1] = len & 0xFF;
  
      int lenWriting = writeBlock(1, data, uid, uidLength);
  
      /*if (lenWriting == 0) {
        Serial.println("Se escribio longitud");
      } else {
        Serial.println("NO se escribio longitud");
      }
  
  
      Serial.println("Final sector");
      Serial.println(finalSector);*/
  
      int res = blocksToWrite % 2;
      int finalBlock = 0;
      if (res == 0) {
        finalBlock = finalSector * 4 + 1;
      } else {
        finalBlock = finalSector * 4;
      }
  
      /*Serial.println("Este es el bloque final");
      Serial.println(finalBlock);*/
  
      for (int sector = 1; sector <= finalSector; sector++) {
        for (int idxBlock = 0; idxBlock < 2; idxBlock++) {
  
          int block = sector * 4 + idxBlock;
  
          if (block <= finalBlock) {
            /*Serial.println("Se escribio en el bloque");
            Serial.println(block);*/
            uint8_t dataBytes[16];
            //Serial.println("Mensaje a escribir");
            for (int j = 0; j < 16; j++) {
              if (idxChar < len) {
                //Serial.print(mensaje[idxChar]);
                dataBytes[j] = mensaje[idxChar];  // Copia los caracteres
                idxChar++;
              } else {
                dataBytes[j] = 0x00;  // Rellena con 0x00 si no hay suficientes caracteres
              }
            }
            //Serial.println("");
            int resultWriteBlock = writeBlock(block, dataBytes, uid, uidLength);
            if (resultWriteBlock != 0) {
              return resultWriteBlock;
            }
          }
        }
      }
    }
  
    return 0;
  }
  
  
  void read(void) {
    Serial.println("Acerque una tarjeta NFC para leer...");
  
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
  
    // Espera hasta que se detecte una tarjeta NFC
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
      //Serial.println("Etiqueta detectada!");
      
      uint8_t * data = readBlock(1,uid, uidLength);
  
      int len =  (data[0] << 8) | data[1];
      //Serial.println("Esta es la longitud");
      //Serial.println(len);
  
      char *  mensaje = (char*) calloc(len, sizeof(char));
  
      int idxChar = 0;
  
      Serial.println("Longitud del mensaje");
      Serial.println(len);
  
      int blocksToRead = (int)ceil(len / 16.0);
      //Serial.println("Cantidad de bloques a leer");
      //Serial.println(blocksToRead);
      int finalSector = (int)ceil(blocksToRead / 2.0);
  
      if (finalSector > 15) {
        return;
      }
  
      //Serial.println("Final sector");
      //Serial.println(finalSector);
  
      int res = blocksToRead % 2;
      int finalBlock = 0;
      if (res == 0) {
        finalBlock = finalSector * 4 + 1;
      } else {
        finalBlock = finalSector * 4;
      }
  
      //Serial.println("Este es el bloque final");
      //Serial.println(finalBlock);
  
      for (int sector = 1; sector <= finalSector; sector++) {
        for (int idxBlock = 0; idxBlock < 2; idxBlock++) {
  
          int block = sector * 4 + idxBlock;
  
          if (block <= finalBlock) {
            //Serial.println("Se escribio en el bloque");
            //Serial.println(block);
            
            //Serial.println("Leyendo mensaje");
            data = readBlock(block,uid, uidLength);
  
            for (int j = 0; j < 16; j++) {
              if (idxChar < len) {
                //Serial.print(mensaje[idxChar]);
                mensaje[idxChar] = (char) data[j];  // Copia los caracteres
                idxChar++;
              } 
            }
            //Serial.println("idxChar");
            //Serial.println(idxChar);
          }
        }
      }
  
      /*lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Mensaje:");
      lcd.setCursor(0,1);
      lcd.print(mensaje);*/
      Serial.println("Mensaje");
      Serial.println(mensaje);
  
    }
}

bool detectCard() {
  Serial.println("Acerque una tarjeta NFC para leer...");

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  // Espera hasta que se detecte una tarjeta NFC
  return nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
}