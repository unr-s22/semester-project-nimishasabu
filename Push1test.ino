//4/24/21 02:01pm
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <DHT.h>

#define DHTPIN 6
#define DHTTYPE DHT11

DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); 
Stepper myStepper(512, 2,3,4,5);
DHT dht(DHTPIN, DHTTYPE);

int Contrast = 60;
float tempIn = 0;
float tempF = 0;
float humidIn = 0;
int state = 1;
int resval = 0;                  // holds the sensor value
int respin = A5;                 // sensor pin used
const int stepButton = 53;
int stepButtonState = 0;
const int fan = A4;
int fanState = 0;
int steppos = 0;
volatile int onOffButton = 18;
volatile int resetButton = 3;
int resetButtonState = 0;
volatile int yLED = 51;
volatile int bLED = 53;
volatile int rLED = 47;
volatile int gLED = 49;

void setup()
{
  analogWrite(13, Contrast);
  lcd.begin(16,2);
  dht.begin();
  rtc.begin();                         // Initialize the rtc object
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
  attachInterrupt(digitalPinToInterrupt(onOffButton), disabled, CHANGE);
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
     // Error state
     while (resval<=100 || resetButtonState != 0) {
      digitalRead(resetButton);
      lcd.clear();
      lcd.display();
      lcd.setCursor(0,0);
      digitalWrite(rLED, LOW);
      lcd.print("Water Lvl: Empty"); 
      delay(1000);
      lcd.clear();
     }

    // Read and display Temp and Humid. Determine state run and idle
    humidIn = dht.readHumidity();
    tempIn = dht.readTemperature();
    tempF = dht.readTemperature(true);
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.setCursor(7,0);
    lcd.print(tempF);
    delay(1000);
    
    // state change to idle from running
    if (tempIn < 80.0) {
      state = 2;
    }
    
    lcd.setCursor(0,1);
    lcd.print("Humid: ");
    lcd.setCursor(8,1);
    lcd.print(humidIn);
    delay(1000);
    lcd.clear(); 

    // Stepper Motor, should move independent of rest, not when disabled
    int stepButtonState = digitalRead(stepButton);
    if (stepButtonState == HIGH){myStepper.step(100); steppos += 1;}
    delay(1000);
    lcd.clear();
}
}

void disabled() {
  // YELLOW LED ON, FAN OFF, No sensors
  state = 0;
  digitalWrite(yLED, LOW);
  lcd.noDisplay();
}

void Times(){
  // During state transition, send this date and time
  Serial.print("Time:  ");
  Serial.println(rtc.getTimeStr());
  Serial.print("Date: ");
  Serial.println(rtc.getDateStr());
  Serial.print("Vent Position: ");
  Serial.print(steppos);
  delay(1000);
}

void idle(){
  // GREEN LED ON, CURRENT TIME, CURRENT WATER LEVEL 
  digitalWrite(gLED, LOW);
}

void error(){
  // RED LED ON, ERROR MESSAGE, Motor off
  digitalWrite(rLED,LOW);
}
