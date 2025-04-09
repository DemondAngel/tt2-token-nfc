#include <ReaderWriterNFC.h>
#include <Wire.h>
#include <math.h>

ReaderWriter::ReaderWriter(Adafruit_PN532 nfc): _nfc(nfc) {}

void ReaderWriter::initPN532(){
    Wire.begin();
    _nfc.begin();

    uint32_t versiondata = _nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("No se encontro el PN532");
        while (1)
        ;  // Detiene la ejecución
    }

    _nfc.SAMConfig();
}

int ReaderWriter::writeBlock(int block, uint8_t* dataBytes, uint8_t* uid, uint8_t uidLength) {

    // Intentamos autenticar usando la clave A
    if (_nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 1, _key)) {
      Serial.println(F("Autenticación exitosa."));
  
      // Escribir datos en el bloque
      if (_nfc.mifareclassic_WriteDataBlock(block, dataBytes)) {
        Serial.println(F("Datos escritos correctamente."));
      } else {
        Serial.println(F("Error al escribir en el bloque."));
        return 1;
      }
    } else {
      //Serial.println("Error de autenticación.");
      return 2;
    }
  
    return 0;
  }
  
  uint8_t * ReaderWriter::readBlock(int block, uint8_t * uid, uint8_t uidLength) {
    static uint8_t data[16];
  
    if (_nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 1, _key)) {
      Serial.println(F("Autenticación exitosa."));
  
      int success = _nfc.mifareclassic_ReadDataBlock(block, data);  // Leer Bloque 1
      if (success) {
        Serial.println(F("Datos leídos del Bloque :"));
        //Serial.println(data);
  
      } else {
        Serial.print(F("Error al leer el Bloque: "));
        Serial.println(block);
      }
    } else {
      //Serial.println("Error de autenticación.");
    }
  
    return data;
  
  }
  
  int ReaderWriter::_writeUuidGeneric(const char * uuid, int sector) {
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    if (_nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {

      uint8_t data[16];

      for(int i = 0; i < 2; i++){
        for(int j = 0; j < 16; j++){
          data[j] = (uint8_t) uuid[j+16*i];
        }
        if(writeBlock(i+(sector*4), data, uid, uidLength) == 1){
          return 1;
        }
      }

      return 0;
    }
    else
      return 3;
  }

  int ReaderWriter::writeUuidCard(const char * uuidCard){
    return _writeUuidGeneric(uuidCard, SECTOR_UUID_CARD);
  }

  int ReaderWriter::writeUuidTokensVersion(const char * uuidTokensVersion){
    return _writeUuidGeneric(uuidTokensVersion, SECTOR_UUID_TOKENS_VERSION);
  }

  int ReaderWriter::writeUuidSharedKey(const char * uuidSharedKey){
    return _writeUuidGeneric(uuidSharedKey, SECTOR_UUID_SHARED_KEY);
  }

  int ReaderWriter::writeToken(uint8_t * token, size_t lengthToken) {
  
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
  
    if (_nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
  
      int idxChar = 0;
  
      //Serial.println("Longitud del mensaje");
      //Serial.println(len);
  
      int blocksToWrite = (int) ceil(lengthToken / 16.0);
      //Serial.println("Cantidad de bloques a escribir");
      //Serial.println(blocksToWrite);
      int sectorsToWrite = (int) ceil(blocksToWrite / 3.0);
      int finalSectorToWrite = sectorsToWrite +2;
      if (finalSectorToWrite > FINAL_SECTOR) {
        return 10;
      }
  
      uint8_t data[16] = { 0 };  // Inicializar el arreglo de datos con ceros
  
      // Convertir el entero a bytes y guardarlo en el arreglo
      data[0] = (lengthToken >> 8) & 0xFF;  // Primer byte
      data[1] = lengthToken & 0xFF;
  
      int lenWriting = writeBlock(2, data, uid, uidLength);
  
      if (lenWriting == 0) {
        Serial.println("Se escribio longitud");
      } else {
        Serial.println("NO se escribio longitud");
      }
  
  
      Serial.println("Final sector");
      Serial.println(sectorsToWrite);
  
      int res = blocksToWrite % 3;
      int finalBlock = 0;
      if (res == 0) {
        finalBlock = finalSectorToWrite * 4 + 2;
      } else {
        finalBlock = (finalSectorToWrite * 4 - 1) + blocksToWrite;
      }
  
      /*Serial.println("Este es el bloque final");
      Serial.println(finalBlock);*/
  
      for (int sector = INIT_SECTOR_TOKEN; sector <= finalSectorToWrite; sector++) {
        for (int idxBlock = 0; idxBlock < 3; idxBlock++) {
  
          int block = sector * 4 + idxBlock;
  
          if (block <= finalBlock) {
            /*Serial.println("Se escribio en el bloque");
            Serial.println(block);*/
            uint8_t dataBytes[16];
            //Serial.println("Mensaje a escribir");
            for (int j = 0; j < 16; j++) {
              if (idxChar < lengthToken) {
                //Serial.print(mensaje[idxChar]);
                dataBytes[j] = token[idxChar];  // Copia los caracteres
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
  
  
  void ReaderWriter::readToken(void) {
    Serial.println("Acerque una tarjeta NFC para leer...");
  
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
  
    // Espera hasta que se detecte una tarjeta NFC
    if (_nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
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
  
      if (finalSector > FINAL_SECTOR) {
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

bool ReaderWriter::detectCard() {
  Serial.println("Acerque una tarjeta NFC para leer...");

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  // Espera hasta que se detecte una tarjeta NFC
  return _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
}