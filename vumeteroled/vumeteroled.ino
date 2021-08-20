#include "display.c"
#include <OLED_I2C.h> // Library: OLED_I2C http://www.rinkydinkelectronics.com/library.php?id=79


OLED  myOLED(SDA, SCL, 8); // Arduino Nano SDA - pin A4, SCL - A5

#define analogInput 0
#define HighSens true 
#define FASTADC true
#define Sensitivity 1024
#define SampleWindow 15

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


int hMeter = 65;                      
int vMeter = 85;                      
int rMeter = 80;                 

int sample;

void setup() {

  if (HighSens == true) { 
    analogReference(INTERNAL);  
  } else {
    analogReference(DEFAULT);
  }

#if FASTADC
  // set prescale to 16
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  cbi(ADCSRA, ADPS0) ;
#endif

  myOLED.begin();          
  myOLED.clrScr();                               
}

void loop() {

  unsigned long startMillis = millis();                    // start of sample window
  unsigned int PeaktoPeak = 0;                             // peak-to-peak level
  unsigned int SignalMax = 0;
  unsigned int SignalMin = Sensitivity;

  while ( millis() - startMillis < SampleWindow ) {

    sample = analogRead(analogInput);
    if (sample < Sensitivity) {

      if (sample > SignalMax) {

        SignalMax = sample;                              
      }

      else if (sample < SignalMin) {

        SignalMin = sample;                              
      }
    }
  }

  PeaktoPeak = SignalMax - SignalMin;                      // max - min = peak-peak amplitude
  float MeterValue = PeaktoPeak * 330 / Sensitivity;              // convert volts to arrow information

  MeterValue = MeterValue - 34;                         
  //myOLED.clrScr();                                // refresh display for next step
  myOLED.drawBitmap(0, 0, VUMeter, 128, 64);       // draws background
  int a1 = (hMeter + (sin(MeterValue / 57.296) * rMeter)); // meter needle horizontal coordinate
  int a2 = (vMeter - (cos(MeterValue / 57.296) * rMeter)); // meter needle vertical coordinate
  myOLED.drawLine(a1, a2, hMeter, vMeter);         // draws needle
  myOLED.update();
}
