/*
# Function:  An MQTT client that connects to a remote QMTT broker.
# Data:      Data transmitted is "Unique ID + UTC + Message"
# Author:    Arnold L. Villasanta
# Date:      December 1, 2021
# Version:   Alpha 1.0
*/

//=======================================================================
//                    LIBRARIES
//=======================================================================

#include "WiFi.h"
#include "time.h"
#include "PubSubClient.h"
#include "EspMQTTClient.h"



//=======================================================================
//                    PARAMETERS
//=======================================================================


// PARAMS:  WiFi
const char* ssid = "A-Triune_2.4G";
const char* password = "SanwalDawn1234";


// PARAMS:  NTP Server (Network Time Protocol)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;


// PARAMS:  MAC of ESP32
uint32_t chipId = 0;
uint64_t chipid;
uint16_t chip;
char chipid_high[100];
char chipid_low[100];
String DEVICE_ID;
char chipssid[23];


//PARAMS:  LED
int ledPin = 15;
String ledState = "OFF";


//=======================================================================
//                    FUNCTION:  MQTT Setup
//=======================================================================

EspMQTTClient client(
  "192.168.254.100",   // MQTT Broker server ip
  1883,                // The MQTT port, default to 1883. this line can be omitted
  "user1",             // Can be omitted if not needed
  "pass1",             // Can be omitted if not needed
  "CommandModule"      // Client name that uniquely identify your device
);


//=======================================================================
//                    MAIN:  SETUP
//=======================================================================

void setup() {

  Serial.begin(115200);
  connectToWiFi();

  //configure NTP parameters (GMT and Daylight Saving)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //configure PINs
  pinMode(ledPin, OUTPUT);

}




//=======================================================================
//                    MAIN:  FUNCTION
//=======================================================================


void loop() {

  client.loop();

  getUTC();

  sendData();

  delay(3000);

}



//=======================================================================
//                    FUNCTION:  Connect to WiFi
//=======================================================================

void connectToWiFi() {

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  delay(5);

}



//=======================================================================
//                    FUNCTION:  Get Coordinated Universal Time
//=======================================================================

String getUTC() {

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "NULL";
  }
  Serial.println();
  char utc[30];
  strftime(utc, sizeof(utc), "%B %d, %Y - %X ", &timeinfo);
  Serial.println(utc);
  return utc;

  delay(5);

}



//=======================================================================
//                    FUNCTION:  Get Machine's Unique ID
//=======================================================================

String getMacId() {

  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  Serial.print("Chip ID: "); Serial.println(chipId);


  //GET THE CHIP_ID AND ASSIGN TO DEVICE_ID
  chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  sprintf(chipid_high, "%04X", (uint16_t)(chipid >> 32)); //print High 2 bytes
  sprintf(chipid_low, "%08X", (uint32_t)chipid); //print Low 4bytes.
  DEVICE_ID = "TelesteCommand-";
  DEVICE_ID += chipid_high;
  DEVICE_ID += chipid_low;

  return DEVICE_ID;

  delay(5);

}



//=======================================================================
//                    FUNCTION:  Get Data  (16 Bytes/Characters minimum)
//=======================================================================

String getSensorData() {

  if (ledState == "OFF") {
    ledState = "ON";
    Serial.println("LED is " + ledState);
    return "LED = on";
  } else if (ledState == "ON") {
    ledState = "OFF";
    Serial.println("LED is " + ledState);
    return "LED = off";
  }

}



//=======================================================================
//                    FUNCTION:  MQTT Connection Activities
//=======================================================================

void onConnectionEstablished()
{

  // Publish
  client.publish("messageData/", "Teleste Command module started"); // You can activate the retain flag by setting the third parameter to true

}


//=======================================================================
//                    FUNCTION:  Send data to MQTT server
//=======================================================================

void sendData() {
  
  String completeData = getMacId() + " | " + getUTC() + " | " + getSensorData();
  Serial.println(completeData);
  client.publish("alertData/", completeData, true);  // retain flag = true

}
