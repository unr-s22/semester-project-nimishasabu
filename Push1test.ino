#include <LiquidCrystal.h>

#include "DHT.h"

#include <RTClib.h>

LiquidCrystal lcd(10, 9, 7, 6, 5, 4);

int Contrast = 60;
float tempIn = 0;
float tempF = 0;
float humidIn = 0;

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;

int resval = 0;  // holds the value
int respin = A5; // sensor pin used

void setup()
{
  analogWrite(11, Contrast);
  lcd.begin(16,2);
  dht.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
}

void loop() {
    if (! rtc.begin()) {
  lcd.display();
  lcd.print("Couldn't find RTC");
  lcd.clear();
}
    
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

    lcd.display();
    lcd.setCursor(0,0);
    DateTime now = rtc.now();
    lcd.print("Date & Time: ");
    lcd.setCursor(0,1);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(" (");
    lcd.print(now.dayOfTheWeek());
    lcd.print(") ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.println(now.second(), DEC);
    lcd.clear();
 
  delay(1000);
}
