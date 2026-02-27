// ##############################################
// #
// # Solar Sensor - SKR v0.4
// #
// # last update 27.02.2026
// #
// ##############################################


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"
// Secrets file reflection

// const char* WIFI_SSID      = "SSID";
// const char* WIFI_PASSWORD  = "PASS";
// const char* NTP_SERVER     = "pl.pool.ntp.org";
// const char* QUESTDB_USER   = "USER";
// const char* QUESTDB_PASS   = "PASS";
// const char* URL_TEMPLATE   = "http://192.168.1.1:9000/exec?query=INSERT%20INTO%20solar_dev_measure%20%28measuretime%2C%20voltage%29%20VALUES%20%28now%28%29%2C{VOLT}%29";

// Constants

const int     PIN_LED = 2;
const int     PIN_ANALOG = A0;
const int     READ_PULSE = 600000; // Read / push waiting time in miliseconds : 600000 ms = 10 min
// const int  READ_PULSE = 2000;
const int     AVERAGE_PULSE = 250; // Averaging wait time in ms
const int     AVERAGE_COUNT = 10; // Number of everaging samples
const float   CALIBRATION = 0.91; // Voltage calibration
const int     NTP_TIME_OFFSET = 3600;

const bool    DEBUG = true;

// Global variables

int sensor = 0;
HTTPClient httpClient;
WiFiClient wifiClient; 
WiFiUDP ntpUDP;
String url;
NTPClient timeClient(ntpUDP, NTP_SERVER);

void blink(int delayTime) {
  for(int i = 0; i < 5; i++)
  {
    digitalWrite(PIN_LED, HIGH);
    delay(delayTime);
    digitalWrite(PIN_LED, LOW);
    delay(delayTime);
  }
  digitalWrite(PIN_LED, HIGH);
}

void blink() {
  blink(50);
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

void log(String message)
{
  if (DEBUG)
  {
    if (timeClient.isTimeSet())
    {
      String formattedtime = getFormattedTimestamp(timeClient.getEpochTime());
      Serial.printf("%s - ", formattedtime.c_str());  
    }
    else
    {
      Serial.print("******************* - ");
    }

    Serial.print(message);
  }
}

void setup() {

  // Configuration
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_ANALOG, INPUT);

  // Wifi connect
  delay(500);

  log("*** SOLAR SENSOR SKR ***\n");
  log("SSID: " + String(WIFI_SSID) + "\n");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  log("Connecting to WiFi \n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    blink();
  }

  log("Connected to WiFi with IP: " + WiFi.localIP().toString() + "\n");

  // Connect to NTP to get local time
  log(F("Connecting to NTP...\n"));

  timeClient.begin();
  timeClient.setTimeOffset(NTP_TIME_OFFSET);
  timeClient.update();

  

  String currentTime = getFormattedTimestamp(timeClient.getEpochTime());
  log("Current NTP time: " + currentTime + "\n");

  // Check free memory of the device
  size_t freeHeap = ESP.getFreeHeap();
  log("Free Heap (SRAM) Memory: " + String(freeHeap) + "[bytes]\n");

  // LED off
  digitalWrite(PIN_LED, HIGH);

}

void loop() {

  // Check WiFi
  while (WiFi.status() != WL_CONNECTED) {
    log("Wi‑Fi lost – trying to reconnect");
    WiFi.reconnect();
    delay(500);
    blink();
  }


  // Read sensor
  sensor = 0;
  for (int i = 0; i < AVERAGE_COUNT; i++)
    sensor += analogRead(PIN_ANALOG);
  sensor = sensor / AVERAGE_COUNT * 3.3 / 1024 * 1000 * CALIBRATION;


  // Print measure to console
  log("Measured voltage: " + String(sensor) + " [mV]\n");

  // Push message to QuestDB
  url = URL_TEMPLATE;
  url.replace("{VOLT}", String(sensor));
  log("Calling URL: " + url + "\n");
  
  httpClient.begin(wifiClient, url);  
  httpClient.setAuthorization(QUESTDB_USER, QUESTDB_PASS);
  int httpCode = httpClient.GET();

  if (httpCode > 0) {        
    log("HTTP GET code: " + String(httpCode) + "\n");
    String payload = httpClient.getString();   
    log("HTTP response: " + payload + "\n");
  } else 
  {
    // log("HTTP error: " + httpClient.errorToString(httpCode).c_str());
    log("HTTP error: " + httpClient.errorToString(httpCode));
  }

  httpClient.end(); 

  // Finalize and wait
  blink(100);
  delay(READ_PULSE);


}

