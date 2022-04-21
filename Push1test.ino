//4/21/21 01:08pm
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Stepper.h>

#include "DHT.h"

DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); 
Stepper myStepper(512, 2,3,4,5);

int Contrast = 60;
float tempIn = 0;
float tempF = 0;
float humidIn = 0;
int state;

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int resval = 0;  // holds the value
int respin = A5; // sensor pin used
const int stepButton = 53;
int stepButtonState = 0;
const int fan = A4;
int fanState = 0;
volatile int onOffButton = 18;
volatile int resetButton = 3;
volatile int yLED = 51;
const int bLED = 53;
const int rLED = 47;
const int gLED = 49;

void setup()
{
  analogWrite(13, Contrast);
  lcd.begin(16,2);
  dht.begin();
  rtc.begin(); // Initialize the rtc object
  rtc.setTime(12,34,30);
  rtc.setDate(4,12,2022);
  myStepper.setSpeed(200);
  pinMode(stepButton, INPUT);
  pinMode(onOffButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT);
  pinMode(yLED, OUTPUT);
  pinMode(bLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(rLED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(onOffButton), disabled, LOW);
}

void loop() {
  digitalWrite(yLED, HIGH);
  digitalWrite(gLED, HIGH);
  digitalWrite(rLED, HIGH);
  digitalWrite(bLED, HIGH);
  // Water Sensor

  if (state==0){
    digitalWrite(yLED, LOW);
    lcd.noDisplay();
    fanState = 0;
  }
  else {
    lcd.clear();
    lcd.display();
    lcd.setCursor(0,0);
    if  (resval<=100) {lcd.print("Water Lvl: Empty");} 
    else if (resval>100 && resval<=300) {lcd.print("Water Lvl: Lw"); } 
    else if (resval>300 && resval<=330) {lcd.print("Water Lvl: Md"); } 
    else if (resval>330) {lcd.print("Water Lvl: Full"); }
    delay(1000); 
    lcd.clear();

  // Read and display Temp and Humid. Determine state run and idle
  humidIn = dht.readHumidity();
  tempIn = dht.readTemperature();
  tempF = dht.readTemperature(true);
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.setCursor(7,0);
  lcd.print(tempF);
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Humid: ");
  lcd.setCursor(8,1);
  lcd.print(humidIn);
  delay(1000);
  lcd.clear(); 

  // Stepper Motor, should move independent of rest, not when disabled
  int stepButtonState = digitalRead(stepButton);
  if (stepButtonState == HIGH){myStepper.step(100);}
  delay(1000);
  lcd.clear();
}

void disabled() {
  // YELLOW LED ON, FAN OFF, No sensors
  digitalWrite(yLED, LOW);
  lcd.noDisplay();
}

 void idle(){
  //GREEN LED ON, CURRENT TIME, CURRENT WATER LEVEL 
  digitalWrite(gLED, LOW);
}

void Times(){
  // During state transition, send this date and time
  lcd.setCursor(0,0);
  lcd.print("Time:  ");
  lcd.println(rtc.getTimeStr());
  lcd.setCursor(0,1);
  lcd.print("Date: ");
  lcd.print(rtc.getDateStr());
  delay(1000);

}

void error(){
  //RED LED ON, ERROR MESSAGE, Motor off
  digitalWrite(rLED,LOW);
}

}
