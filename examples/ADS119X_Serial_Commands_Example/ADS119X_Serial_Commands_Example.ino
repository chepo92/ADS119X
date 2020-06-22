/*
Code developed by Axel Sepulveda github: @chepo92 
Tested on Arduino UNO, but should be compatible with any board supported by Arduino IDE

Wiring: 
Conect SPI wires (MOSI, MISO, SCLK), according to your board
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

This library detects the device model and channel number automatically (ADS119X, with X = [4,6,8])

Connect the channel inputs to your measured signal, remember ADS119X have differential inputs only. 

This code initiates the ADS119X following the "quick start guide" flow in the datasheet
First, after adc.begin() the ADC is conected to an internal test signal of about 0.9 Hz (if using the internal oscillator)
then the device is configured to a normal measurement state (external inputs), reading in continuous mode, with:
Gain = 1, sample rate = 250 SPS, it will measure all channels
and sent the raw data out (signed 16bit integer) over serial at 250000 baud, each channel separated by comma,  
so you can se it in the serial  monitor or in the serial plotter

It also allows to enable, disable and send commands over serial with format n,m   (with new line enabled) 
to control and write to the ADS's registers

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
boolean verbosity = 0; 
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

    // Stop continuous conversion
    adc.sendCommand (ADS119X_CMD_SDATAC);
    adc.setAllChannelGain( ADS119X_CHnSET_GAIN_1);  
    adc.setAllChannelMux(ADS119X_CHnSET_MUX_NORMAL); 
    adc.setDataRate(ADS119X_DRATE_250SPS);     
    adc.sendCommand (ADS119X_CMD_RDATAC);
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

   recvWithEndMarker();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseData();
    showParsedData();
    newData = false;
      if (command[0] == 0) {
          // Stop continuous conversion
          adc.sendCommand (ADS119X_CMD_SDATAC);  
          state = 0;
          if (command[1] == 0) {
              adc.syncRegData() ;
              displayRegs ();      
          } 
          if (command[1] == 1) {
            adc.setAllChannelGain( ADS119X_CHnSET_GAIN_1);  
            adc.setAllChannelMux(ADS119X_CHnSET_MUX_NORMAL); 
            displayRegs ();      
          }
          if (command[1] == 10) {         
            adc.testSignal();
            displayRegs ();      
          }                
          
      }  else if (command[0] == 1) {
          //    RDATAC
          adc.sendCommand (ADS119X_CMD_RDATAC);
          state = 1;
      } else if (command[0] == 10) {
          //    RDATAC
          adc.sendCommand (ADS119X_CMD_RDATAC);
          state = 1;
      } 
  }


}
