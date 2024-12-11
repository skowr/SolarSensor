
int sensor = 0;
int pinLed = 2;
const int pinAnalog = A0;

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinAnalog, INPUT);
  Serial.begin(115200);
}


void blink() {
  digitalWrite(pinLed, HIGH);  
  delay(100);
  digitalWrite(pinLed, LOW);   
  delay(100);
}


void loop() {
  // blink();

  sensor = analogRead(pinAnalog);
  Serial.println(sensor);
  delay(1000);
}