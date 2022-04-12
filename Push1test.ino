#include <DS3231.h>
#include <LiquidCrystal.h>

#include "DHT.h"

DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(10, 9, 7, 6, 5, 4);

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
 
}

void loop() {
    resval = analogRead(respin);
    lcd.display();
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
 lcd.println(rtc.getTimeStr());
 
 lcd.setCursor(0,1);
 lcd.print("Date: ");
 lcd.println(rtc.getDateStr());
 
 delay(1000);
}
