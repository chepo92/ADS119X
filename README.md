# ADS119X
ADS119X library compatible with Arduino IDE 
Compatible IC: ADS1194, ADS1196 and ADS1198

## Installing
Download and install zip library in Arduino IDE 

## Compatible Microcontrollers
Any board supported in Arduino IDE should work

## Dependencies
This library uses SPI class

## Usage examples
The library comes with 3 examples
- Test internally generated signal
- Basic
- Serial Commands

## Wiring
Connect SPI's pins (MISO, MOSI, SCLK), additionally the user can select DRDY, RST, and CS pins (example's default are pins 6, 5 and 10 with arduino UNO) 

## Boards known to have an ADS119X
- ECG2 Click from MikroE uses a ADS1194 (this library is tested with this breakout board)

- Bare chips are sold in Aliexpress


## Status: Working as June 2020
