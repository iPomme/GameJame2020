#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <driver/dac.h>
//#include <ArduinoJson.h>
#include <WebSocketsServer.h>

#define SERVER_PORT 8989

struct sensorData_t{
  float quatI;
  float quatJ;
  float quatK;
  float quatReal;
};

union Websock_Packet_t{
    sensorData_t sensor;
    byte bytePacket[sizeof(sensorData_t)];
};

Websock_Packet_t wsd;  
Websock_Packet_t wsd_last;  
void print_Values (int16_t *gyro, int16_t *accel, int32_t *quat, uint32_t *timestamp);