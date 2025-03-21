#ifndef STORE_DATA_H
#define STORE_DATA_H
#include <SharedKey.h>
#include <Arduino.h>

char * getToken();
void storeToken(char * token);

SharedKey getSingleSharedKey(char * uuid_shared_key);
SharedKey * getSharedKeys();
void writeSharedKey(SharedKey sharedKey);

//Crear métodos para recuperar usuario y contrasenia del lector

#endif
