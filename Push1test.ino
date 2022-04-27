//4/27/21 3:44 pm
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

//const int fan = 45; volatile const int yLED = 53; B0
//volatile const int bLED = 51; B2 volatile const int rLED = 47; L2 volatile const int gLED = 49; L0
unsigned char *port_B = (unsigned char *) 0x25;  // for yLED and bLED
unsigned char *DDR_B = (unsigned char *) 0x24;
unsigned char *port_L = (unsigned char *) 0x10B; // for fan rLED and gLED
unsigned char *DDR_L = (unsigned char *) 0x10A;
//unsigned char *port_D = (unsigned char *) 0x2B;
//unsigned char *DDR_D = (unsigned char *) 0x2A;
int Contrast = 60;
volatile float tempIn = 0;
volatile float tempF = 0;
volatile float humidIn = 0;
int disabledState = 1;

int resval = 0;                  // holds the sensor value
int respin = A5;                 // sensor pin used
const int stepButton = 53;
int stepButtonState = 0;
int steppos = 0;
volatile int onOffButton = 18;
volatile int resetButton = 3;
int resetButtonState = 0;
void disabled();

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

  *DDR_B |= 0b00000001 | 0b00000100;   //yled | bled - output ;
  *DDR_L |= 0b00010000 | 0b00000001 | 0b00000100; //fan | gled | rled - output;
 
  attachInterrupt(digitalPinToInterrupt(onOffButton), disabled, CHANGE);
}

void loop() {
  *port_B &= (0 << PINB0) & (0 << PINB2); // set yled and bled to low
  *port_L &= (0 << PINL4) & (0 << PINL0) & (0 << PINL2); // set fan gled rled to low

    humidIn = dht.readHumidity();
    tempIn = dht.readTemperature();
    tempF = dht.readTemperature(true);
  // Water Sensor
  
  if (disabledState==1){
    *port_B = (1 << PINB0); // digitalWrite(yLED, HIGH);
    *port_L &= (0 << PINL4); //Fan to low
    lcd.noDisplay();
    delay(100);
  }
  else {//IDLE State
    *port_B &= (0 << PINB0); // yellow led off
    *port_L |= (1 << PINL0);//turn the green led on
     lcd.clear();
     lcd.display();
     lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.setCursor(7,0);
    lcd.print(tempIn);
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Humid: ");
    lcd.setCursor(8,1);
    lcd.print(humidIn);
    delay(1000);
    lcd.clear();
     
    //*port_L = 0b00010001; //(1 << PINL0) | (1 << PINL4); *port_L = (1 << PINL4); *port_L = (1 << PINL0);
    
    while (tempIn > 30.0) {//running state
      *port_L &= (0 << PINL0);// turn the green led off
      *port_B |= (1 << PINB2);//turn the blue led on
      *port_L |= (1 << PINL4); //turn the fan on
  }


  }
    
//     resval = analogRead(respin);
//     
//     // Error state
//     while (resval<=100 || resetButtonState != 0) {
//      digitalRead(resetButton);
//      lcd.clear();
//      lcd.display();
//      lcd.setCursor(0,0);
//      *port_L |= (0 << PINL2); // digitalWrite(rLED, HIGH);
//      lcd.print("Water Lvl: Empty"); 
//      delay(1000);
//      lcd.clear();
     
//
//    // Read and display Temp and Humid. Determine state run and idle
//    humidIn = dht.readHumidity();
//    tempIn = dht.readTemperature();
//    tempF = dht.readTemperature(true);
//    lcd.setCursor(0,0);
//    lcd.print("Temp: ");
//    lcd.setCursor(7,0);
//    lcd.print(tempF);
//    delay(1000);
//    
//    // state change to idle from running
//    if (tempIn < 80.0) {
//      state = 2;
//    }
//    
//    lcd.setCursor(0,1);
//    lcd.print("Humid: ");
//    lcd.setCursor(8,1);
//    lcd.print(humidIn);
//    delay(1000);
//    lcd.clear(); 
//
//    // Stepper Motor, should move independent of rest, not when disabled
//    int stepButtonState = digitalRead(stepButton);
//    if (stepButtonState == HIGH){myStepper.step(100); steppos += 1;}
//    delay(1000);
//    lcd.clear();

}


void disabled() {
  // YELLOW LED ON, FAN OFF, No sensors
  if (disabledState == 0) {disabledState = 1;}
  else {disabledState = 0;}
}

//void Times(){
//  // During state transition, send this date and time
//  Serial.print("Time:  ");
//  Serial.println(rtc.getTimeStr());
//  Serial.print("Date: ");
//  Serial.println(rtc.getDateStr());
//  Serial.print("Vent Position: ");
//  Serial.print(steppos);
//  delay(1000);
//}
//
