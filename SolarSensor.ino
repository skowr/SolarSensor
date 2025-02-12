#include <ESP8266WiFi.h>
#include "time.h"
#include "secrets.h"

int sensor = 0;
int pinLed = 2;
const int pinAnalog = A0;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

void blink() {
  digitalWrite(pinLed, HIGH);  
  delay(100);
  digitalWrite(pinLed, LOW);   
  delay(100);
}

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinAnalog, INPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("SSID:");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  char *now = getLocalTime();
  // printf("%s\n", now);
}


char *getLocalTime(){
  struct tm timeinfo;

  // char *ret = (char *) malloc(sizeof(char) * 20); // allocate memory for the string
  // if(!getLocalTime(&timeinfo)){
  //   Serial.println("Failed to obtain time");
  //   return;
  // }

  char retTime[20];

  // 2025.01.01 15:15:45

  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  // Serial.print("Day of week: ");
  // Serial.println(&timeinfo, "%A");
  // Serial.print("Month: ");
  // Serial.println(&timeinfo, "%B");
  // Serial.print("Day of Month: ");
  // Serial.println(&timeinfo, "%d");
  // Serial.print("Year: ");
  // Serial.println(&timeinfo, "%Y");
  // Serial.print("Hour: ");
  // Serial.println(&timeinfo, "%H");
  // Serial.print("Hour (12 hour format): ");
  // Serial.println(&timeinfo, "%I");
  // Serial.print("Minute: ");
  // Serial.println(&timeinfo, "%M");
  // Serial.print("Second: ");
  // Serial.println(&timeinfo, "%S");


  // Serial.println("Time variables");
  char timeYear[4];
  strftime(timeYear, 4, "%Y", &timeinfo);
  
  strcat(retTime, timeYear);

  // char timeHour[3];
  // strftime(timeHour,3, "%H", &timeinfo);




  // Serial.println(timeHour);
  // char timeWeekDay[10];
  // strftime(timeWeekDay,10, "%A", &timeinfo);
  // Serial.println(timeWeekDay);
  // Serial.println();


  return retTime;
}


void loop() {
  blink();

  sensor = analogRead(pinAnalog);
  Serial.println(sensor);
  delay(1000);
}