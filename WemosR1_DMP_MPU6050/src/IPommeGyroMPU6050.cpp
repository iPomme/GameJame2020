#include <Arduino.h>
#include "IPommeGyroMPU6050.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
//#include "MPU_ReadMacros.h"
//#inc;ude "MPU_WriteMacros.h"
#include "Simple_MPU6050.h"
#include "mdns.h"
#include <ESP32Ticker.h>

Ticker secTimer;
Ticker minTimer;

// Client
bool isConnected=false;


#define MPU6050_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    0x69 // address pin high (VCC)
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_LOW

Simple_MPU6050 mpu;
TaskHandle_t mpuRunningTask;
//ENABLE_MPU_OVERFLOW_PROTECTION();
/*             _________________________________________________________*/
//               X Accel  Y Accel  Z Accel   X Gyro   Y Gyro   Z Gyro
//#define OFFSETS  -5260,    6596,    7866,     -45,       5,      -9  // My Last offsets. 
//              X Accel  Y Accel  Z Accel   X Gyro   Y Gyro   Z Gyro
#define OFFSETS  -5584,    -400,    1240,     108,      77,      -6
//       You will want to use your own as these are only for my specific MPU6050.
/*             _________________________________________________________*/

//***************************************************************************************
//******************                Print Funcitons                **********************
//***************************************************************************************

#define spamtimer(t) for (static uint32_t SpamTimer; (uint32_t)(millis() - SpamTimer) >= (t); SpamTimer = millis()) // (BLACK BOX) Ya, don't complain that I used "for(;;){}" instead of "if(){}" for my Blink Without Delay Timer macro. It works nicely!!!

/* printfloatx() is a helper Macro used with the Serial class to simplify my code and provide enhanced viewing of Float and interger values:
   usage: printfloatx(Name,Variable,Spaces,Precision,EndTxt);
   Name and EndTxt are just char arrays
   Variable is any numerical value byte, int, long and float
   Spaces is the number of spaces the floating point number could possibly take up including +- and decimal point.
   Percision is the number of digits after the decimal point set to zero for intergers
*/
#define printfloatx(Name,Variable,Spaces,Precision,EndTxt) print(Name); {char S[(Spaces + Precision + 3)];Serial.print(F(" ")); Serial.print(dtostrf((float)Variable,Spaces,Precision ,S));}Serial.print(EndTxt);//Name,Variable,Spaces,Precision,EndTxt

int PrintRealAccel(int16_t *accel, int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  VectorInt16 aa, aaReal;
  spamtimer(SpamDelay) {// non blocking delay before printing again. This skips the following code when delay time (ms) hasn't been met
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.SetAccel(&aa, accel);
    mpu.GetLinearAccel(&aaReal, &aa, &gravity);
    Serial.printfloatx(F("aReal x")  , aaReal.x , 9, 4, F(",  ")); //printfloatx is a Helper Macro that works with Serial.print that I created (See #define above)
    Serial.printfloatx(F("y")        , aaReal.y , 9, 4, F(",  "));
    Serial.printfloatx(F("z")        , aaReal.z, 9, 4, F(",  "));
    Serial.println("");
  }

  return 1;
}

bool detected[10];
bool circle_completed[4];
unsigned long millisec;

/*Function to find maximum of x and y*/
    int my_max(int x, int y) 
    { 
        return x ^ ((x ^ y) & -(x < y));  
    } 
    
 int hula_max_diameter=0;


int PrintWorldAccel(int16_t *accel, int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  VectorInt16 aa, aaReal, aaWorld;
   
  spamtimer(SpamDelay) {// non blocking delay before printing again. This skips the following code when delay time (ms) hasn't been met
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.SetAccel(&aa, accel);
    mpu.GetLinearAccel(&aaReal, &aa, &gravity);
    mpu.GetLinearAccelInWorld(&aaWorld, &aaReal, &q);
//    if(aaWorld.x > 5000 && !detected[0]) {Serial.print("x > detected : "); hula_max_diameter = my_max(aaWorld.x,hula_max_diameter); circle_completed[0] = true; Serial.printfloatx(F("x")  , aaWorld.x , 9, 4, F(",   ")); detected[0]=true;} else detected[0]=false;  
//    if(aaWorld.y > 5000 && !detected[1]) {Serial.print("y > detected : ");  hula_max_diameter = my_max(aaWorld.y,hula_max_diameter); circle_completed[1] = true;  Serial.printfloatx(F("y")        , aaWorld.y, 9, 4, F(",   ")); detected[1]=true;} else detected[1]=false;  
    if(aaWorld.x > 5000 && !detected[0]) {Serial.print("x > detected : "); hula_max_diameter = my_max(aaWorld.x,hula_max_diameter);  detected[0]=true;}  
    if(aaWorld.y > 5000 && !detected[1]) {Serial.print("y > detected : ");  hula_max_diameter = my_max(aaWorld.y,hula_max_diameter);  detected[1]=true;}  
    //if(aaWorld.z > 3000 && !detected[2]) {Serial.print("z > detected : ");  Serial.printfloatx(F("z")        , aaWorld.z, 9, 4, F(",  ")); detected[2]=true;} else detected[2]=false; 
//    if(aaWorld.x < -5000 && !detected[3]) {Serial.print("-x < detected : ");  hula_max_diameter = my_max(aaWorld.x*-1,hula_max_diameter); circle_completed[2] = true;  Serial.printfloatx(F("x")  , aaWorld.x , 9, 4, F(",   ")); detected[3]=true;} else detected[3]=false;  
//    if(aaWorld.y < -5000 && !detected[4]) {Serial.print("-y < detected : ");  hula_max_diameter = my_max(aaWorld.x*-1,hula_max_diameter); circle_completed[3] = true;  Serial.printfloatx(F("y")        , aaWorld.y, 9, 4, F(",   ")); detected[4]=true;} else detected[4]=false;  
    if(aaWorld.x < -5000 && !detected[2]) {Serial.print("-x < detected : ");  hula_max_diameter = my_max(aaWorld.x*-1,hula_max_diameter);  detected[2]=true;} 
    if(aaWorld.y < -5000 && !detected[3]) {Serial.print("-y < detected : ");  hula_max_diameter = my_max(aaWorld.x*-1,hula_max_diameter); detected[3]=true;} 
    //if(aaWorld.z < -2000 && !detected[5]) {Serial.print("-z < detected : ");  Serial.printfloatx(F("z")        , aaWorld.z, 9, 4, F(",  ")); detected[5]=true;} else detected[5]=false;
     
    if(detected[0]&&detected[1]&&detected[2]&&detected[3])
    {
      wsd.sensor.hula_count++;
      wsd.sensor.hula_diameter = hula_max_diameter;
      hula_max_diameter=0;
      detected[0] =false;detected[1] =false;detected[2] =false;detected[3] =false;  
      if(millis() > millisec) wsd.sensor.hula_speed = millis()-millisec;
      millisec = millis();
      Serial.print("  -- COMPLETION TIME: "); Serial.print(wsd.sensor.hula_speed);
      Serial.print("  -- MAX CIRCONFERENCE: "); Serial.print(wsd.sensor.hula_diameter);
      Serial.print("  -- HULA COUNTS: "); Serial.print(wsd.sensor.hula_count);
      Serial.println("");
    }

    //Serial.printfloatx(F("aWorld x")  , aaWorld.x , 9, 4, F(",   ")); //printfloatx is a Helper Macro that works with Serial.print that I created (See #define above)
    //Serial.printfloatx(F("y")        , aaWorld.y, 9, 4, F(",   "));
    //Serial.printfloatx(F("z")        , aaWorld.z, 9, 4, F(",  "));
  }


  return 1;
}


int PrintQuaternion(int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  spamtimer(SpamDelay) {// non blocking delay before printing again. This skips the following code when delay time (ms) hasn't been met
    mpu.GetQuaternion(&q, quat);
    Serial.printfloatx(F("quat w")  , q.w, 9, 4, F(",   ")); //printfloatx is a Helper Macro that works with Serial.print that I created (See #define above)
    Serial.printfloatx(F("x")       , q.x, 9, 4, F(",   "));
    Serial.printfloatx(F("y")       , q.y, 9, 4, F(",   "));
    Serial.printfloatx(F("z")       , q.z, 9, 4, F("\n"));
  }
  return 1;
}



/*********************************************************************/
// WiFi
// Client

WebSocketsClient webSocketClient1;
WiFiMulti wfMulti;
int netw=0;
char ip_str[20];

void mpuRunningTaskCode( void * parameter) {
   for(;;) {
        webSocketClient1.loop();
    if(isConnected)
    {
     mpu.dmp_read_fifo();// Must be in loop
     mpu.OverflowProtection();
    }
   }

 }


bool posChanged()
{
  bool pc = false;
  if(abs(wsd_last.sensor.quatI-wsd.sensor.quatI) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.quatJ-wsd.sensor.quatJ) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.quatK-wsd.sensor.quatK) >= 0.001) pc = true;
  if(abs(wsd_last.sensor.quatReal-wsd.sensor.quatReal) >= 0.001) pc = true;
  
  wsd_last.sensor.quatI=wsd.sensor.quatI;
  wsd_last.sensor.quatJ=wsd.sensor.quatJ;
  wsd_last.sensor.quatK=wsd.sensor.quatK;
  wsd_last.sensor.quatReal=wsd.sensor.quatReal;

  return(pc);
}

void onMinTimer()
{
  /*
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
      Serial.println();
      */
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
			Serial.printf("[WSc] Disconnected!\n");
      Serial.println("");
      isConnected =false;
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
      Serial.println("");
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

IPAddress local_IP(192, 168, 43, 11);
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
    mdns_hostname_set("GyroJoy2");
    //set default instance
    mdns_instance_name_set("Beat's GyroJoy2");
    Serial.printf("MDNS2 Started");
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

   // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin(D4,D3,400000);
  //Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

#ifdef OFFSETS
  Serial.println(F("Using Offsets"));
  mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).load_DMP_Image(OFFSETS); // Does it all for you

#else
  Serial.println(F(" Since no offsets are defined we aregoing to calibrate this specific MPU6050,\n"
                   " Start by having the MPU6050 placed stationary on a flat surface to get a proper accellerometer calibration\n"
                   " Place the new offsets on the #define OFFSETS... line at top of program for super quick startup\n\n"
                   " \t\t\t[Press Any Key]"));
  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available());                 // wait for data
  while (Serial.available() && Serial.read()); // empty buffer again
  mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).CalibrateMPU().load_DMP_Image();// Does it all for you with Calibration
#endif
  mpu.on_FIFO(print_Values);
  
  // https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
  xTaskCreatePinnedToCore(
      mpuRunningTaskCode, /// Function to implement the task 
      "Task1", // Name of the task 
      10000,  // Stack size in words 
      NULL,  // Task input parameter 
      1,  // Priority of the task 
      &mpuRunningTask,  // Task handle. 
      1); // Core where the task should run 
 
  secTimer.attach(1, onSecTimer);
  minTimer.attach(60, onMinTimer);


	
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
  //resolve_mdns_host("android-51a93c0b018d06a5");
  //resolve_mdns_host("FRODO");
	//webSocketClient1.begin(ip_str, 8989, "/corona");
	webSocketClient1.begin("192.168.43.234", 8989, "/corona");

	// event handler
	webSocketClient1.onEvent(webSocketClientEvent);
	//webSocket.setAuthorization("user", "Password");
	webSocketClient1.setReconnectInterval(5000);
 	isConnected = true;
}

void loop()
{
 
}


//***************************************************************************************
//******************              Callback Funciton                **********************
//***************************************************************************************

void print_Values (int16_t *gyro, int16_t *accel, int32_t *quat, uint32_t *timestamp) {
  uint8_t Spam_Delay = 100; // Built in Blink without delay timer preventing Serial.print SPAM
  Quaternion q;
  mpu.GetQuaternion(&q, quat);
  //PrintRealAccel(accel, quat,  Spam_Delay);
  PrintWorldAccel(accel, quat, Spam_Delay);
 
  wsd.sensor.quatI=q.x;
  wsd.sensor.quatJ=q.y;
  wsd.sensor.quatK=q.z;
  wsd.sensor.quatReal=q.w;
  if(posChanged())
    {
      if(isConnected) 
      {
        webSocketClient1.sendBIN(wsd.bytePacket, sizeof(sensorData_t));

      }

      //PrintQuaternion(quat, Spam_Delay);
    }  
}
