/// 5/4/2022 3:16 pm
/// CPE 301 Swamp Cooler Project
/// Written by: Jeremy Laporte, Nimisha Sabu, Yeamin Chowdery
/// const int fan = 45; volatile const int yLED = 53; (B0)
/// volatile const int bLED = 51; (B2) volatile const int rLED = 47; (L2) volatile const int gLED = 49; (L0)
/// const int stepButton = 50 (B3); //volatile int onOffButton = 18 (D3); //volatile int resetButton = 38 (D7);
/// input: DDr = 0; inw/pullup: DDr = 0, port = 1, pin = 1; output: DDr = 1;

#include <LiquidCrystal.h>  // LCD display library
#include <Stepper.h>        // Stepper motor library
#include <DHT.h>            // Temp and Humidity sensor library
#include <RTClib.h>         // Real time clock library

RTC_DS1307 rtc;             // Create real time clock object

#define DHTPIN 6            // Set DHT sensor to pin 6
#define DHTTYPE DHT11       // Set DHT to type 11 from library

LiquidCrystal lcd(12, 11, 10, 9, 8, 7); 
Stepper myStepper(512, 2, 3, 4, 5);
DHT dht(DHTPIN, DHTTYPE);

unsigned char *port_B = (unsigned char *) 0x25;  // for yLED and bLED and stepButton
unsigned char *DDR_B = (unsigned char *) 0x24;
unsigned char *port_L = (unsigned char *) 0x10B; // for fan rLED and gLED
unsigned char *DDR_L = (unsigned char *) 0x10A;
unsigned char *port_D = (unsigned char *) 0x2B; // for onOff and reset button
unsigned char *DDR_D = (unsigned char *) 0x2A;
unsigned char *pin_D = (unsigned char *) 0x29;  // for input w/ pullup

volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;   // Analog to digital mux
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;  // anatodig serial reg B
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;  // A to D serial reg A
volatile unsigned int* my_ADCA = (unsigned int*) 0x78;  // A to D data register



//External Interrupt Register addresses.
volatile unsigned char* EICRA_1 = (unsigned char*) 0x69;  // Interrupt control register A
volatile unsigned char* EICRB_1 = (unsigned char*) 0x6A;  // Interrupt control register B
volatile unsigned char* EMISK_1 = (unsigned char*) 0x3D;  // Interrupt mask
volatile unsigned char* EIFR_1 = (unsigned char*) 0x3C;   // Interrupt flag register
volatile unsigned char* SREG_1 = (unsigned char*) 0x3F;   // Status register
 
int Contrast = 60;              // lCD contrast   
volatile float tempIn = 0;      // temperature reading
volatile float tempF = 0;       // temperature Farenheit value
volatile float humidIn = 0;     // humidity reading
volatile int disabledState = 1; // Error state variable
int stepButtonState = 0;        
int steppos = 0;
volatile int resetState = 0;    // Reset button/Esc-Error pressed?
int count1 = 0;                 // Serial-single-print disabled flag
int count2 = 0;                 // Serial-single-print idle flag
int count3 = 0;                 // Serial-single-print error flag
int count4 = 0;                 // Serial-single-print running flag
void print_time();
unsigned int adc_read(unsigned char adc_channel_num);
void adc_init();
// prototype the function for calling
unsigned int resval = 0;                 // holds the sensor value
//unsigned char int respin = A5;                // analog water sensor - pin used


void setup()
{
  analogWrite(13, Contrast);
  Serial.begin(9600);
  lcd.begin(16,2);                     // Initialize the lcd display
  dht.begin();                         // Initialize the temperature sensor
  rtc.begin();                         // Initialize the real time clock object
  myStepper.setSpeed(200);             // Set stepper motor speed to constant
  adc_init();                          // Initialize the analog to digital converter

  // pin modes:
  *DDR_B |= 0b00000001 | 0b00000100;              // yled | bled - output
  *DDR_L |= 0b00010000 | 0b00000001 | 0b00000100; // fan | gled | rled - output
  *DDR_D &= 0b01110111;                           // resetButton | onOffButton - input
  *DDR_B &= 0b11110111;                           // stepMotor - input

  // 
  *EICRA_1 |= 0b00000001;                         // set interrupt trigger to CHANGE
  *EMISK_1 |= 0b00001000;                         // sets PD3 for the interrupt
  *SREG_1 |= 0b10000000;                          // sets initial status reg value
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // initially find and set the time and date
}


void loop() {
  // All States:
  resetState = 0; // keeps the reset state to 0 periodically to prevent accidental trigger
  *port_B &= (0 << PINB0) & (0 << PINB2);                // set yled and bled to low
  *port_L &= (0 << PINL4) & (0 << PINL0) & (0 << PINL2); // set fan gled rled to low
  
  // Read and display Temp and Humid. Determine state run and idle
  humidIn = dht.readHumidity();
  tempIn = dht.readTemperature();
  tempF = dht.readTemperature(true);
  resval = adc_read(5);
    
  if (disabledState==1){
   // DISABLED State
    // During state transition, send this date and time once
    if (count1 == 0) {
      print_time();
      delay(500);
    }
    count1 = 1; count2 = 0; count3 = 0; count4 = 0;
    *port_B = (1 << PINB0); // digitalWrite(yLED, HIGH);
    *port_L &= (0 << PINL4); // Fan to low
    lcd.noDisplay();
    delay(100);
  }
  
  else {
   // IDLE/RUNNING/ERROR State hub
    // During state transition, send this date and time once
    if (count2 == 0) {
      print_time();
      delay(500);
    }
    count1 = 0; count2 = 1; count3 = 0; count4 = 0;
    *port_B &= (0 << PINB0); // yellow led off
    *port_L |= (1 << PINL0); // turn the green led on
    // Stepper Motor, should move independent of rest, not when disabled
    stepButtonState = digitalRead(53);
    if (stepButtonState == HIGH){
      myStepper.step(100); 
      steppos += 1;
    }
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
    delay(1000);

    while(resval <= 300 && disabledState==0 && resetState == 0){
     //ERROR State
      // During state transition, send this date and time once
      if (count3 == 0) {
        print_time();
        delay(500);
      }
      count1 = 0; count2 = 0; count3 = 1; count4 = 0;
      // Stepper Motor, should move independent of rest, not when disabled
      stepButtonState = digitalRead(53);
      if (stepButtonState == HIGH){myStepper.step(100); steppos += 1;}
      
      resetState=!digitalRead(38);
      *port_L &= (0 << PINL0); //turn off the green light
      *port_L &= (0 << PINL4); //keep the fan off 
      *port_L |= (1 << PINL2); //turn the red light on
      delay (1000);
      lcd.clear();
      lcd.display();
      lcd.setCursor(0,0);
      lcd.print("Water Lvl: LOW");}
      
    while (tempIn > 22.0 && disabledState==0 && resval>300) {
     //RUNNING State
      // During state transition, send this date and time once
      if (count4 == 0) {
        print_time();
        delay(500);
      }
      count1 = 0; count2 = 0; count3 = 0; count4 = 1;
      *port_L &= (0 << PINL0); // turn the green led off
      *port_B |= (1 << PINB2); // turn the blue led on
      *port_L |= (1 << PINL4); // turn the fan on
      // Stepper Motor, should move independent of rest, not when disabled
      stepButtonState = digitalRead(53);
      if (stepButtonState == HIGH){myStepper.step(100); steppos += 1;}
      humidIn = dht.readHumidity();
      tempIn = dht.readTemperature();
      tempF = dht.readTemperature(true);
      resval = adc_read(5);
      lcd.display();
      lcd.print(resval);
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
      delay(1000);
    }
  }
}


// ------------------------------------------------------------------------
ISR (INT3_vect) {
  if (disabledState == 0) {disabledState = 1;}
  else {disabledState = 0;}
}

// ------------------------------------------------------------------------
void print_time(){
   DateTime now = rtc.now();
   Serial.print('\n');
   Serial.print("TIME");
   Serial.print(" ");
   Serial.print(now.hour());
   Serial.print(":");
   Serial.print(now.minute());
   Serial.print(":");
   Serial.print(now.second());
   Serial.print(" ");
   Serial.print('\n');
   Serial.print("DATE");
   Serial.print(" ");
   Serial.print(now.day());
   Serial.print('/');
   Serial.print(now.month());
   Serial.print('/');
   Serial.print(now.year());
   Serial.print("  "); 
   delay(500); 
}

// ------------------------------------------------------------------------
void adc_init()
{
  // set up the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 3 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 2-0 to 0 to set prescaler selection to slow reading
  
  // set up the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  
  // set up the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // reset the channel and gain bits
  *my_ADMUX = 0x00;
  
  // clear the channel selection bits
  *my_ADCSRB = 0x00;
  
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    
    // set MUX bit 3
    *my_ADCSRB |= 0b00001000;
  }
  
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  
  // set bit ?? of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0b01000000;
  
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  
  // return the result in the ADC data register
  return *my_ADCA;
}
