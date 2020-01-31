#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <driver/dac.h>
//#include <ArduinoJson.h>
#include <WebSocketsServer.h>

#define SSID "Pagaie"
#define PASSWORD "87654321"
#define SERVER_PORT 8989

typedef struct sensorData_t{
  float quatI;
  float quatJ;
  float quatK;
  float quatReal;
  byte mostLikelyActivity;
  uint16_t jx;
  uint16_t jy;
  uint16_t jz;
};

typedef union Websock_Packet_t{
    sensorData_t sensor;
    byte bytePacket[sizeof(sensorData_t)];
};

Websock_Packet_t wsd;  
Websock_Packet_t wsd_last;  
