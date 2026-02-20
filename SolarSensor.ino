// ##############################################
// #
// # Solar Sensor - SKR v0.1
// #
// # last update 20.02.2026
// #
// ##############################################


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

const int pinLed = 2;
const int PIN_ANALOG = A0;
const int READ_PULSE = 600000; // Read / push waiting time in miliseconds
// const int READ_PULSE = 2000;
const int AVERAGE_PULSE = 250;
const int AVERAGE_COUNT = 10;

const int NTP_TIME_OFFSET = 3600;

// const char* URL_TEMPLATE = "http://192.168.1.1:9000/exec?query=INSERT%20INTO%20solar_dev_measure%20%28measuretime%2C%20voltage%29%20VALUES%20%28now%28%29%2C{VOLT}%29";

int sensor = 0;
HTTPClient httpClient;
WiFiClient wifiClient; 
WiFiUDP ntpUDP;
String url;
NTPClient timeClient(ntpUDP, NTP_SERVER);

void blink() {
  for(int i = 0; i < 5; i++)
  {
    digitalWrite(pinLed, HIGH);
    delay(50);
    digitalWrite(pinLed, LOW);
    delay(50);
  }
}

String getFormattedTimestamp(unsigned long long epochTime) {

  struct tm *ptm = gmtime((time_t *)&epochTime);

  int currentYear = ptm->tm_year + 1900;

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
  Serial.begin(9600);

  pinMode(pinLed, OUTPUT);
  pinMode(PIN_ANALOG, INPUT);


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

  // Connect to NTP to get local time
  Serial.println(F("Connecting to NTP..."));

  timeClient.begin();
  timeClient.setTimeOffset(NTP_TIME_OFFSET);
  timeClient.update();

  // String formattedTime = timeClient.getFormattedTime();
  // Serial.print(F("NTP Connected. "));
  // Serial.println(formattedTime);

  String currentTime = getFormattedTimestamp(timeClient.getEpochTime());

  Serial.print(F("Current time: "));
  Serial.println(currentTime);


  // Check free memory of the device
  size_t freeHeap = ESP.getFreeHeap();
  Serial.print(F("Free Heap (SRAM) Memory: "));
  Serial.println(freeHeap);

  digitalWrite(pinLed, LOW);

}

void loop() {

  // Read sensor
  sensor = 0;
  for (int i = 0; i < AVERAGE_COUNT; i++)
    sensor += analogRead(PIN_ANALOG);
  sensor = sensor / AVERAGE_COUNT * 3.3 / 1024 * 1000;


  String formattedtime = getFormattedTimestamp(timeClient.getEpochTime());

  // Print result to console
  unsigned long epochTime;
  epochTime = timeClient.getEpochTime();
  String timestamp = String(epochTime);

  Serial.print(formattedtime);  
  Serial.print(" - ");
  Serial.print(sensor);
  Serial.println(" [mV]");

  // if (WiFi.status() != WL_CONNECTED) {
  //   Serial.println("Wi‑Fi lost – trying to reconnect");
  //   WiFi.reconnect();
  //   delay(2000);
  //   return;
  // }


  // Push message to QuestDB
  url = URL_TEMPLATE;
  url.replace("{VOLT}", String(sensor));
  Serial.println(url);

  /*

  httpClient.begin(wifiClient, url);
  
  int httpCode = httpClient.GET();

  if (httpCode > 0) {        
    Serial.printf("[HTTP] GET code: %d\n", httpCode);
    String payload = httpClient.getString();   
    Serial.print("[HTTP] Response: ");
    Serial.println(payload);
  } else {                   
    Serial.printf("[HTTP] GET... failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
  }

  httpClient.end(); 
*/

  delay(READ_PULSE);
  blink();
}

