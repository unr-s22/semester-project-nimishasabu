//4/14/22 11:27pm
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Stepper.h>

#include "DHT.h"


DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(10, 9, 7, 6, 5, 4);
const int stepsPerRevolution = 300;  
Stepper myStepper(stepsPerRevolution, 14, 15, 16, 17);

int Contrast = 60;
float tempIn = 0;
float tempF = 0;
float humidIn = 0;

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int resval = 0;  // holds the value
int respin = A5; // sensor pin used

void setup()
{
  analogWrite(11, Contrast);
  lcd.begin(16,2);
  dht.begin();
  rtc.begin(); // Initialize the rtc object
  rtc.setTime(12,34,30);
  rtc.setDate(4,12,2022);
}

void loop() {
    resval = analogRead(respin);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0,0);
    if  (resval<=100) {lcd.print("Water Lvl: Empty");} 
    else if (resval>100 && resval<=300) { lcd.print("Water Lvl: Lw"); } 
    else if (resval>300 && resval<=330) { lcd.print("Water Lvl: Md"); } 
    else if (resval>330) {lcd.print("Water Lvl: Full"); }
    delay(1000); 
    lcd.clear();

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
  
 lcd.setCursor(0,0);
 lcd.print("Time:  ");
 lcd.print(rtc.getTimeStr());
 
 lcd.setCursor(0,1);
 lcd.print("Date: ");
 lcd.print(rtc.getDateStr());
 
 delay(1000);

int sensorReading = analogRead(A4);
 int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
 
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution / 100);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pval");
  lcd.setCursor(6, 1);
  lcd.print(motorSpeed);
  lcd.print(" %");
  
  delay(1000);
  
    }
