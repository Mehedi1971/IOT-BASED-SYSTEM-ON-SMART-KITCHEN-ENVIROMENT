#define BLYNK_TEMPLATE_ID "TMPL4PIqqWAF"
#define BLYNK_DEVICE_NAME "Smart Kitchen"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial

#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#define APP_VPIN_GAS V2
#define APP_VPIN_TEMP V1

#include "BlynkEdgent.h"
#include <TM1637.h>
#include <SimpleDHT.h>


int pinDHT11 = D5;
byte temperature = 10, humidity = 10;
int motion;
int CLK = D1;
int DIO = D2;
int GAS_VOUT = A0;
int gasMeter = 0;
int Buzzer = D8;
int pirSensor = D0;
int greenLight = D7;

SimpleDHT11 dht11(pinDHT11);
TM1637 tm(CLK,DIO);
BlynkTimer timer;

void ShowTempDisplay(int SenData) {
  if(SenData == 0) {
    return;
  }
  int var = SenData;
  int tmp[4];
  tmp[1] = var % 10;
  var /= 10;
  tmp[0] = var % 10;
  
  for(int i=0; i<2; i++) {
    tm.display(i,tmp[i]); 
  }
  tm.display(2,12);
}

void PIRtoLED(int mtn) {
  if(mtn){
    Serial.println("|");
    digitalWrite(greenLight,HIGH);
  } else{
    Serial.println(".");
    digitalWrite(greenLight,LOW);
  }
}

void myTimerEvent()
{
  dht11.read(&temperature, &humidity, NULL);
  Blynk.virtualWrite(APP_VPIN_GAS, gasMeter);
  Blynk.virtualWrite(APP_VPIN_TEMP, (int)temperature);
}

void BuzzAndNotify() {
  if(gasMeter >= 400) {
    tone(Buzzer, 261);
    Blynk.logEvent("smoke_warning");
  } else {
    noTone(Buzzer);
  }
}

void setup() {
  pinMode(pirSensor,INPUT);
  pinMode(greenLight,OUTPUT);
  pinMode(GAS_VOUT, INPUT);
  pinMode(Buzzer, OUTPUT);

  Serial.begin(115200);
  
  BlynkEdgent.begin();

  tm.set(2);
  tm.init();

  timer.setInterval(1000L, myTimerEvent);
  timer.setInterval(5000L, BuzzAndNotify);
}

void loop() {
  gasMeter = analogRead(GAS_VOUT);

  BlynkEdgent.run();
  timer.run();
  
  PIRtoLED(digitalRead(pirSensor));
  ShowTempDisplay((int)temperature);
}