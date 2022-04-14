//4/14/22 12:10pm
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Stepper.h>

#include "DHT.h"


DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
const int stepsPerRevolution = 300;  
Stepper myStepper(stepsPerRevolution, 2,3,4,5);

int Contrast = 60;
float tempIn = 0;
float tempF = 0;
float humidIn = 0;

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int resval = 0;  // holds the value
int respin = A5; // sensor pin used

void setup()
{
  analogWrite(13, Contrast);
  lcd.begin(16,2);
  dht.begin();
  rtc.begin(); // Initialize the rtc object
  rtc.setTime(12,34,30);
  rtc.setDate(4,12,2022);
  myStepper.setSpeed(60);

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

int Potval = analogRead(A4);
if (Potval>0 && Potval <= 50){
    myStepper.setSpeed(analogRead(A4)/10);
    myStepper.step(200);}
else if (Potval>50 && Potval< 100){
  myStepper.setSpeed(analogRead(A4)/10);
    myStepper.step(-200);}


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pval");
  lcd.setCursor(6, 1);
  lcd.print(analogRead(A4));
  lcd.print(" %");
  

  
  delay(1000);
