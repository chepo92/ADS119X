/*
Code developed by Axel Sepulveda github: @chepo92 
Tested on Arduino UNO, but should be compatible with any board supported by Arduino IDE

Wiring: 
Connect SPI wires (MOSI, MISO, SCLK), according to your board
Additionally you need to connect to any pin you want dataReady, reset and CS (or SS, spi slave select), default pins are configured as follows 
dataReady_Pin   to pin 6
reset_pin       to pin 5
cs_pin          to pin 10 

            WARNING
see the datasheet for supply connection details and max values

ADS119X are digital 3.3V devices and analog 5V devices

DVDD: connect to 3.3V (do not conect to 5V as may destroy the chip)
AVDD: connect to 5V or 3.3V 
GND to ground


You don't need to connect the inputs as they will be connected to an internal test square wave 0.9Hz

This library detects the device model and channel number automatically (ADS119X, with X = [4,6,8])

This code initiates the ADS119X following the "quick start guide" flow in the datasheet
After adc.begin() the ADC wil be connected to an internal test signal of about 0.9 Hz (if using the internal oscillator)
and sent the raw data out (signed 16bit integer) over serial at 250000 baud, each channel separated by comma,  
so you can se it in the serial  monitor or in the serial plotter 

*/


#include <SPI.h>
#include "ADS119X.h" 

// pins 
byte dataReady_Pin = 6;
byte reset_pin = 5;
byte cs_pin = 10 ; 

ADS119X adc(dataReady_Pin,reset_pin , cs_pin);

// variables
bool dataReady = 0;
bool prev_dataReady = 0;
byte received ; 
boolean verbosity = 1; 
byte numberOfChannels;

// Serial comm
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
int command [2]; 
boolean newData = false;

byte state = 1; 

int elapsed = 0 ;
long prevMicros = 0 ;

void setup() {
    Serial.begin(250000);  
    // adc begin() puts the ADS in a datasheet state, if verbosity is true it will print the ADS's register values
    if (adc.begin()) {
      numberOfChannels = adc.getNumberOfChannels();
      if (verbosity) {
        Serial.print("ADS119X Conected") ; 
        Serial.println("") ;  
        displayRegs ();        
        Serial.print("Num Channels: ") ;         
        Serial.print(numberOfChannels) ; 
        Serial.println();  
      }    
    } else {
      Serial.print("ADS119X not started") ; 
    }
    

}

void loop() {

 // Check if the ADC conversion data is ready
 if (state) { 
    dataReady = adc.isDRDY();

    if (!prev_dataReady && dataReady) {
      elapsed = micros() - elapsed  ;
      //Serial.println(elapsed);
      elapsed = micros() ; 
      adc.readChannelData();
//    printStatus();
      printData();
      
    }
    prev_dataReady= dataReady;
 }

}


void printStatus (){
  Serial.print(adc.getStatus(),HEX);
  Serial.print(", ");
}

void printData (){
  for (int ch = 0; ch < numberOfChannels ; ch++)  //adc.getNumberOfChannels()
  {
    Serial.print(adc.getChannelData(ch)); 
    if (ch < numberOfChannels - 1 ) {
      Serial.print(","); 
    }
    
  }
  Serial.println("");
}


void displayRegs (){
  Serial.println("REGISTERS: ");
  for (int i = 0; i < adc.getRegisterSize() ; i++)
  {
    if (i < 0x10) {
      Serial.print(" ") ;
    }
     Serial.print("0x") ;
     Serial.print(i, HEX) ; 
     Serial.print(": ") ;
     print8bits(adc.getRegister(i)) ; 

  }
}

void print8bits(int var) {
  for (unsigned int bitpos = 0x80; bitpos; bitpos >>= 1) {
    Serial.write(var  & bitpos ? '1' : '0');
  }
  Serial.println();
}
