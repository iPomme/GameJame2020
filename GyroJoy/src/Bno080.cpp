#include <Arduino.h>
#include "Bno080.h"
//extern "C" int rom_phy_get_vdd33();
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
#include <ESP32Ticker.h>
#include "mdns.h"

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

WebSocketsClient webSocketClient1;
//WebSocketsClient webSocketClient2;
//WebSocketsClient webSocketClient3;
WiFiMulti wfMulti;
int netw=0;
char ip_str[20];

Ticker secTimer;
Ticker minTimer;


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

void onMinTimer()
{
 /* // https://github.com/espressif/arduino-esp32/issues/2158
   float internalBatReading;
   btStart();
     internalBatReading = rom_phy_get_vdd33();
     wsd.sensor.power =(float)((internalBatReading*2960)/2798); 
   btStop();*/
}

void onSecTimer()
{
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
			//Serial.printf("[WSc] Disconnected!\n");
      isConnected =false;
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			webSocketClient1.sendTXT("Connected");
		//	webSocketClient2.sendTXT("Connected");
		//	webSocketClient3.sendTXT("Connected");
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
    case WStype_PING:  
      break;
    case WStype_PONG:  
      break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

IPAddress local_IP(192, 168, 43, 10);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);

const char* ssid[] = {"frodocat"};
const char* password[] = {"gamejam2020"};

WiFiClient espClient;

void resolve_mdns_host(const char * host_name)
{
    Serial.printf("Query A: %s.local", host_name);

    struct ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host_name, 5000,  &addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            Serial.printf("Host was not found!");
            return;
        }
        Serial.printf("Query Failed");
        return;
    }

  sprintf(ip_str, IPSTR, IP2STR(&addr));
  Serial.printf(IPSTR, IP2STR(&addr));    
}

void start_mdns_service()
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        Serial.printf("MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    mdns_hostname_set("GyroJoy");
    //set default instance
    mdns_instance_name_set("Beat's GyroJoy");
    Serial.printf("MDNS Started");

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  int ccount=0;
  while (WiFi.status() != WL_CONNECTED) 
  {
   if(ccount==0)
   { 
    WiFi.mode(WIFI_OFF);
    delay(2000);
    WiFi.mode(WIFI_STA);
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet)) {
      Serial.println("STA Failed to configure");
    }
    WiFi.begin(ssid[netw], password[netw]);
   } 
   Serial.println();
   Serial.print("Connecting to ");
   Serial.println(ssid[netw]);
   delay(500);
    //Serial.print("w");
    //Serial.print(F("FreeMemory: "));
    //Serial.println(System.freeMemory());
    Serial.println( WiFi.SSID());
    Serial.println( WiFi.RSSI());
    if(WiFi.RSSI()!=0) ccount=0;
    Serial.println( WiFi.localIP());
    ccount++;
    if(ccount > 10)
    {
      ccount =0;
      if(netw < 0) 
      { 
        netw++; 
      }
      else
      {
        netw=0; 
      }
        WiFi.begin(ssid[netw], password[netw]);
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  start_mdns_service();

}

void reconnect() {
  // Loop until we're reconnected
    Serial.println( WiFi.SSID());
    Serial.println( WiFi.RSSI());
    Serial.println( WiFi.localIP());
}

void gotoSleep()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  //WiFi.forceSleepBegin();
  delay(1);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup WiFi");
  setup_wifi();
  Serial.println();
 
  secTimer.attach(1, onSecTimer);
  minTimer.attach(60, onMinTimer);

  Wire.begin();

  if (myIMU.begin(BNO080_DEFAULT_ADDRESS, Wire,INT_PIN) == false)
  {
    Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }

  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  //myIMU.enableRotationVector(50); //Send data update every 50ms
  myIMU.enableGameRotationVector(50);
  myIMU.enableMagnetometer(50);
  Serial.println(F("Rotation vector enabled"));
  Serial.println(F("Output in form i, j, k, real, accuracy"));
  myIMU.enableActivityClassifier(50, enableActivities, activityConfidences);
  myIMU.enableStabilityClassifier(50);
  myIMU.enableStepCounter(50);
   // Joystik
  pinMode(JOYPIN_Z, INPUT);
	
  Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

  //resolve_mdns_host("Hikaru");
  //resolve_mdns_host("Android");
  resolve_mdns_host("FRODO");
	webSocketClient1.begin(ip_str, 8989, "/paddle");

	// event handler
	webSocketClient1.onEvent(webSocketClientEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");
	// try every 5000 ms again if connection has failed
	webSocketClient1.setReconnectInterval(5000);
  
/*  resolve_mdns_host("FRODO");
  webSocketClient2.begin(ip_str, 8989, "/paddle");
	webSocketClient2.onEvent(webSocketClientEvent);
	webSocketClient2.setReconnectInterval(5000);
  resolve_mdns_host("Android");
  webSocketClient3.begin(ip_str, 8989, "/paddle");
	webSocketClient3.onEvent(webSocketClientEvent);
	webSocketClient3.setReconnectInterval(5000);*/

 	isConnected = true;
  pinMode(INT_PIN, INPUT_PULLUP);
}

int oldZ=false;

void loop()
{
  webSocketClient1.loop();
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
    wsd.sensor.stabilityClass = myIMU.getStabilityClassifier();
    wsd.sensor.steps = myIMU.getStepCount();
    
    if(posChanged())
    {
      if(isConnected) 
      {
        webSocketClient1.sendBIN(wsd.bytePacket, sizeof(sensorData_t));

      }

/*
      Serial.print(wsd.sensor.quatReal, 2);
      Serial.print(F(","));
      Serial.print(wsd.sensor.quatI, 2);
      Serial.print(F(","));
      Serial.print(wsd.sensor.quatJ, 2);
      Serial.print(F(","));
      Serial.print(wsd.sensor.quatK, 2);

      Serial.println();
*/

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
      Serial.print("stab:");
      Serial.print(wsd.sensor.stabilityClass, DEC);
      Serial.print(F(","));
      Serial.print("jx:");
      Serial.print(wsd.sensor.jx, DEC);
      Serial.print(F(","));
      Serial.print("jy:");
      Serial.print(wsd.sensor.jy, DEC);
      Serial.print(F(","));
      Serial.print("jz:");
      Serial.print(wsd.sensor.jz, DEC);
      Serial.print(F(","));
      Serial.print("Steps:");
      Serial.print(wsd.sensor.steps, DEC);
      Serial.print(F(","));
      Serial.print("Power:");
      Serial.print(wsd.sensor.power, 4);
      Serial.println();

    }
  }
 }