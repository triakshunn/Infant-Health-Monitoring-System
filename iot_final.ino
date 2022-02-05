#define USE_ARDUINO_INTERRUPTS true 
#include <CayenneMQTTSerial.h>
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11 
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer ;
int sound;
int soundPin = A0;
DHT dht(DHTPIN, DHTTYPE);
char username[] = "a144d1b0-2b51-11ec-9f5b-45181495093e";
char password[] = "885833fcb8fa5f25168bbd0b3f2171ed0168d587";
char clientID[] = "9eee1c10-47cf-11ec-9f5b-45181495093e";
   // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   

//  Variables
const int PulseWire = A3;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550; 
int myBPM;  
float h,X;
float t,room_sound;
int d;
bool humidity = false;
bool Temperature = false;
PulseSensorPlayground pulseSensor;
void setup() {
 Cayenne.begin(username, password, clientID);
 dht.begin();
  humidity = false;
  Temperature = false;
  Wire.begin();
  byte status = mpu.begin();
   pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  while(status!=0){ }
  mpu.calcOffsets();
  calculate_roomsound();
  if (pulseSensor.begin()) {
    digitalWrite(13,HIGH); //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  delay(1000);
}
void loop() {
  mpu.update();
  Cayenne.loop();
         
}
CAYENNE_OUT(V0){
  //Serial.println("humidity");

  //Check if read failed and try until success
  do {
    //Read humidity (percent)
   sound=analogRead(soundPin);;
  } while  (isnan(h));

  //Set Humidity to true so we know when to sleep
  humidity = true;

  Cayenne.virtualWrite(V0, sound-room_sound);
}

CAYENNE_OUT(V1){
  //Serial.println("temperature");
  
  do {
    t = dht.readTemperature();    
  } while  (isnan(t));
  Cayenne.virtualWrite(V1, t,"temp","c");
}
CAYENNE_OUT(V2){
  
  do {
    X =mpu.getAngleX(); 
    timer=millis();  
  } while  (millis()-timer>0);
  Cayenne.virtualWrite(V2, X);
}
CAYENNE_OUT(V3){
   int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int"
  Cayenne.virtualWrite(V3,myBPM);
}
void calculate_roomsound(){
  while(d<200){
    room_sound=room_sound+analogRead(soundPin);
    d++;
  }
  room_sound=room_sound/200;
  d=0;
}
