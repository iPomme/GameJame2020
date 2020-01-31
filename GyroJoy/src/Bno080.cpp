#include <Arduino.h>
#include "Bno080.h"

/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586
  https://github.com/sparkfun/Qwiic_IMU_BNO080/tree/master/Documents


  This example shows how to output the i/j/k/real parts of the rotation vector.
  https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation

*/

#include <Wire.h>
#include <SparkFun_BNO080_Arduino_Library.h>

// BNO080
BNO080 myIMU;
uint16_t enableActivities  = 0x1F;
byte activityConfidences[9];
bool calibrated = false;
bool dataReady = false;
#define INT_PIN 32

// Joystik
#define JOYPIN_Z  35
#define JOYPIN_X  34          
#define JOYPIN_Y  33 

// Client
bool isConnected=false;

WiFiServer server(SERVER_PORT);
WebSocketsClient webSocketClient;
WiFiMulti wfMulti;



bool posChanged()
{
  bool pc = false;
  if(abs(wsd_last.sensor.quatI-wsd.sensor.quatI) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.quatJ-wsd.sensor.quatJ) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.quatK-wsd.sensor.quatK) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.quatReal-wsd.sensor.quatReal) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.mostLikelyActivity-wsd.sensor.mostLikelyActivity) >= 1) pc = true;
  if(abs(wsd_last.sensor.jx-wsd.sensor.jx) >= 10) pc = true;
  if(abs(wsd_last.sensor.jy-wsd.sensor.jy) >= 10) pc = true;
  if(abs(wsd_last.sensor.jz-wsd.sensor.jz) >= 1) pc = true;

  wsd_last.sensor.quatI=wsd.sensor.quatI;
  wsd_last.sensor.quatJ=wsd.sensor.quatJ;
  wsd_last.sensor.quatK=wsd.sensor.quatK;
  wsd_last.sensor.quatReal=wsd.sensor.quatReal;
  wsd_last.sensor.mostLikelyActivity=wsd.sensor.mostLikelyActivity;
  wsd_last.sensor.jx=wsd.sensor.jx;
  wsd_last.sensor.jy=wsd.sensor.jy;
  wsd_last.sensor.jz=wsd.sensor.jz;

  return(pc);
}


void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}

void pharseText( uint8_t * payload, size_t length)
{
 	char buff_p[length];
	for (int i = 0; i < length; i++)
	{
		buff_p[i] = (char)payload[i];
	}
	buff_p[length] = '\0';
	String msg_p = String(buff_p);
}

void webSocketClientEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
      isConnected =false;
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			webSocketClient.sendTXT("Connected");
      isConnected =true;
			break;
		case WStype_TEXT:
			pharseText(payload,length);
			break;
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Read Example");

  Wire.begin();

  if (myIMU.begin(BNO080_DEFAULT_ADDRESS, Wire,INT_PIN) == false)
  {
    Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }

  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  myIMU.enableRotationVector(50); //Send data update every 50ms
  Serial.println(F("Rotation vector enabled"));
  Serial.println(F("Output in form i, j, k, real, accuracy"));
  myIMU.enableActivityClassifier(50, enableActivities, activityConfidences);

   // Joystik
  pinMode(JOYPIN_Z, INPUT);
	Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
	//WiFi.softAPConfig(IPAddress(192,168,5,1),IPAddress(192,168,5,1),IPAddress(255,255,255,0));
  WiFi.softAP(SSID, PASSWORD);
  //Serial.print("Access point running. IP address: ");
  //Serial.println(WiFi.softAPIP());
	Serial.println("Wait 100 ms for AP_START...");
  delay(100);
  
  Serial.println("Set softAPConfig");
  IPAddress Ip(192, 168, 5, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
 
  Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

	webSocketClient.begin("192.168.5.2", 8989, "/paddle");

	// event handler
	webSocketClient.onEvent(webSocketClientEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");
	// try every 5000 ms again if connection has failed
	webSocketClient.setReconnectInterval(5000);
 	isConnected = true;
  pinMode(INT_PIN, INPUT_PULLUP);
}

int oldZ=false;

void loop()
{
  webSocketClient.loop();
  // Get Joystik pos
  wsd.sensor.jx=analogRead(JOYPIN_X);
  wsd.sensor.jy=analogRead(JOYPIN_Y);
  wsd.sensor.jz = digitalRead(JOYPIN_Z);

  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    dataReady=false;
   
    wsd.sensor.quatI = myIMU.getQuatI();
    wsd.sensor.quatJ = myIMU.getQuatJ();
    wsd.sensor.quatK = myIMU.getQuatK();
    wsd.sensor.quatReal = myIMU.getQuatReal();
    wsd.sensor.mostLikelyActivity = myIMU.getActivityClassifier(); 

    if(posChanged())
    {
      if(isConnected) 
      {
        webSocketClient.sendBIN(wsd.bytePacket, sizeof(sensorData_t));
      }
      if(isConnected) Serial.print("connected:");
      Serial.print("quatI:");
      Serial.print(wsd.sensor.quatI, 2);
      Serial.print(F(","));
      Serial.print("quatJ:");
      Serial.print(wsd.sensor.quatJ, 2);
      Serial.print(F(","));
      Serial.print("quatK:");
      Serial.print(wsd.sensor.quatK, 2);
      Serial.print(F(","));
      Serial.print("quatReal:");
      Serial.print(wsd.sensor.quatReal, 2);
      Serial.print(F(","));
      Serial.print("mla:");
      Serial.print(wsd.sensor.mostLikelyActivity, DEC);
      Serial.print(F(","));
      Serial.print("jx:");
      Serial.print(wsd.sensor.jx, DEC);
      Serial.print(F(","));
      Serial.print("jy:");
      Serial.print(wsd.sensor.jy, DEC);
      Serial.print(F(","));
      Serial.print("jz:");
      Serial.print(wsd.sensor.jz, DEC);
      Serial.println();
    }
  }
 }