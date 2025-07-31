#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include "secrets.h"

int sensor = 0;
int pinLed = 2;
const int pinAnalog = A0;
const int readPulse = 1000; // miliseconds

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER);

void blink() {
  digitalWrite(pinLed, HIGH);
  delay(100);
  digitalWrite(pinLed, LOW);
  delay(100);
}

String getFormattedTimestamp(unsigned long long epochTime) {

  Serial.println(epochTime);

  struct tm *ptm = gmtime((time_t *)&epochTime);

  int currentYear = ptm->tm_year + 1900;
  // Serial.print(F("Year: "));
  // Serial.println(currentYear);


  char buffer[30];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
          ptm->tm_year + 1900,
          ptm->tm_mon + 1,
          ptm->tm_mday,
          ptm->tm_hour,
          ptm->tm_min,
          ptm->tm_sec);
  return String(buffer);
}

void setup() {

  // Configuration
  Serial.begin(115200);

  pinMode(pinLed, OUTPUT);
  pinMode(pinAnalog, INPUT);


  // Wifi connect
  delay(500);
  Serial.print(F("\n\nSSID: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);


  Serial.print(F("Connecting to WiFi."));
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.print(F("\nConnected to WiFi. IP: "));
  Serial.println(WiFi.localIP());

  // Connect to NTP
  Serial.println(F("Connecting to NTP..."));



  timeClient.begin();
  timeClient.setTimeOffset(NTP_TIME_OFFSET);
  timeClient.update();



  String formattedTime = timeClient.getFormattedTime();
  Serial.print(F("NTP Connected. Time: "));
  Serial.println(formattedTime);

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  Serial.print(F("Year : "));
  Serial.println(timeinfo.tm_year);


  String timestamp = getFormattedTimestamp(timeClient.getEpochTime());

  Serial.print(F("Timestamp: "));
  Serial.println(timestamp);
}


void loop() {

  String timestamp = getFormattedTimestamp(timeClient.getEpochTime());

  sensor = analogRead(pinAnalog);
  Serial.print(timestamp);
  Serial.print(": ");
  Serial.println(sensor);
  
  
  
  delay(readPulse);
}

// REF
// https://microdigisoft.com/esp8266-nodemcu-ntp-server-create-date-and-time-with-using-arduino-ide/
// https://microdigisoft.com/create-date-and-time-with-ntp-server-using-esp8266-wifi-module/