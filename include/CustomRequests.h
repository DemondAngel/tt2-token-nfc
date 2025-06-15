#ifndef CUSTOM_REQUESTS_H
#define CUSTOM_REQUESTS_H

#include <Card.h>
#include <DeviceInfo.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#if TARGET_PLATFORM == 0
  #include <Ethernet.h>
  const int pinETH = 53;
  const int w5500ResetPin = 16;
const int   w5500InterruptPin = 2;
#else
  #include <WiFi.h>
  #include <HTTPClient.h>
#endif

class CustomRequests {
private:

    #if TARGET_PLATFORM == 0
      const int _pinETH = pinETH; // Pin para control de Ethernet (si aplica)
      const int _rstPin = w5500ResetPin;
      const int _intPin = w5500InterruptPin;
      EthernetClient _client;
      const IPAddress _arduinoIP; // Ahora son miembros const, se inicializan en el constructor
      const IPAddress _proxyIP;
      const int _proxyPort;
      const uint8_t _mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    #else
      HTTPClient _http;
      int _httpCode = 0;
      const char * _ssid;
      const char * _pass;
    #endif

    const char * _HOST;
    
    DeviceInfo &_currentDeviceInfo; // Información del dispositivo cargada

    // Métodos privados para uso interno
    JsonDocument _parseJson();
    void _prepareQuery(const char * method, const char * uri, const char * contentType, bool auth, const char * token, const JsonDocument& docRequest);
    #if TARGET_PLATFORM == 0
      bool _connectToProxy();
    #else
      bool _connectToWiFi();
    #endif

    bool _connect();

    void _disconnect();
    void _authNFC();

public:
    // Constructor - requiere una instancia de StoreData
    #if TARGET_PLATFORM == 0
      CustomRequests(const char * host, const int proxyPort, IPAddress arduinoIP, IPAddress proxyIP, DeviceInfo &currentDeviceInfo);
    #else 
      CustomRequests(const char * host, const char * ssid, const char * pass, DeviceInfo &currentDeviceInfo);
    #endif

    // Métodos públicos para realizar las acciones
    void registerCard(Card& card);
    //void generateToken(Card& card);
    void extractToken(JsonDocument & doc, Card & card);
    bool validateToken(Card& card);
    void getSharedKeyNFC(SharedKey & sharedKey);

    // Getter para la información del dispositivo (opcional)
    DeviceInfo& getCurrentDeviceInfo();
};

#endif