#include "ADS119X.h"
#include "Arduino.h"
#include "SPI.h"

ADS119X::ADS119X(byte dataReady_Pin, byte reset_Pin, byte cs_Pin)
{
  // pins 
  _drdy_pin = dataReady_Pin;
  _reset_pin = reset_Pin;
  _cs_pin = cs_Pin; 
  
  // Set DRDY as input
  pinMode(_drdy_pin, INPUT);      // using arduino functions for more compatibility
  // set RESETPIN as output
  pinMode(_reset_pin, OUTPUT );
  // Set CS as output
  pinMode(_cs_pin, OUTPUT);
  
  // Start SPI
  SPI.begin();
  SPI.beginTransaction( SPISettings(100000, MSBFIRST, SPI_MODE1));
  
}

boolean ADS119X::begin() 
{
    digitalWrite(_reset_pin, LOW ); 
    delay(10);
    digitalWrite(_reset_pin, HIGH ); 
    delay(10);
    sendCommand (ADS119X_CMD_RESET);
    delay(100);
    
    // Stop continuous conversion
    sendCommand (ADS119X_CMD_SDATAC);
    delay(10);
    
    // Read All registers, save locally a make a default copy. 
    syncRegData() ; 
    for (byte adrs = 0 ; adrs < ADS119X_REG_SIZE ; adrs++ )
    {
        _regDataDefault[adrs] = _regData[adrs];
    }

    // If the register corresponds to an ADS119X ID
    if ( (_regData [ADS119X_ADD_ID] & ADS119X_ID_7_2) == ADS119X_ID_7_2) {    
        getNumberOfChannelsFromReg();    
        //datasheet recommended start sequence
        testNoise();
        testSignal();
        //setDefaultSettings();
        //    Send START command if pin Start is tied low in the board
//        sendCommand (ADS119X_CMD_START);        
//        //    Put the Device Back in RDATAC Mode, send RDATAC
//        sendCommand (ADS119X_CMD_RDATAC);
        delay (10);       
        return true;
    } else {
      return false; 
    }
}


void ADS119X::sendCommand(byte _command) 
{
  csLow();    
  xfer(_command);
  csHigh();  

}

void ADS119X::WREG(byte address, byte value)
{                                             //  Write ONE register at _address
  byte opcode1 = address + ADS119X_CMD_WREG; //  WREG expects 010rrrrr where rrrrr = _address
  csLow();                        //  open SPI
  xfer(opcode1);                  //  Send WREG command & address
  xfer(0x00);                     //  opcode2, Send number of registers to write -1
  xfer(value);                    //  Write the value to the register
  csHigh();                       //  close SPI
  _regData[address] = value;       //  update the mirror array
}

byte ADS119X::RREG(byte  address)
{                                               //  reads ONE register at _address
  byte opcode1 =  address + ADS119X_CMD_RREG;   //  RREG expects 001r rrrr where rrrrr = _address
  csLow();                                      //  open SPI
  xfer(opcode1);                                //  opcode1
  xfer(0x00);                                   //  opcode2, Send number of registers to read -1
  _regData[ address] = xfer(0x00);               //  update mirror location with returned byte
  csHigh();                                     //  close SPI

  return _regData[ address]; // return requested register value
}

// Read more than one register starting at _address
void ADS119X::RREGS(byte  address, byte  numRegistersMinusOne)
{
  byte opcode1 =  address + 0x20; //  RREG expects 001rrrrr where rrrrr = _address
  csLow();                        //  open SPI
  xfer(opcode1);                  //  opcode1
  xfer( numRegistersMinusOne);    //  opcode2
  for (int i = 0; i <=  numRegistersMinusOne; i++)
  {
    _regData[ address + i] = xfer(0x00); //  save register byte to mirror array
  }
  csHigh(); //  close SPI
}

void ADS119X::setDataRate(byte dataRate)
{
    // Stop continuous conversion, send SDATAC
    sendCommand (ADS119X_CMD_SDATAC);
    delay(10);
    
    // Set data rate WREG CONFIG1 0x03
    byte valueToWrite = (_regData[ADS119X_ADD_CONFIG1] & ~ADS119X_DRATE_MASK) | dataRate ;
    WREG(ADS119X_ADD_CONFIG1 , valueToWrite);
  
}


void ADS119X::readChannelData()
{
  byte inByte;

  //Read  status 24 bits
  csLow(); //  open SPI
  for (int i = 0; i < 3; i++)
  {
    inByte = xfer(0x00); //  read 24 bit status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
    _boardStat = (_boardStat << 8) | inByte;
  }
  for (int i = 0; i < ADS119X_TOTAL_CH; i++) 
  {    // Read all channels
    for (int j = 0; j < ADS119X_BYTES_PER_CH; j++)
    { //  read 16 bits of channel data in 2 byte chunks
      inByte = xfer(0x00);
      _channelData[i] = (_channelData[i] << 8) | inByte; // int data goes here
    }
  }
  csHigh(); // close SPI
  // No need to convert 16bit to 32bit or anything as channelData is 16 bit int (represented with signed 2'c binary)
}

boolean ADS119X::isDRDY() {
  return !digitalRead(_drdy_pin);
}  

void ADS119X::setChannelSettings(byte _address, byte pwdownSetting, byte gainSetting, byte muxSetting )
{ 
 byte _valueToWrite = keepSetting(_address) & ~(ADS119X_CHnSET_PD_MASK | ADS119X_CHnSET_GAIN_MASK | ADS119X_CHnSET_MUX_MASK) ; 
 _valueToWrite |= (pwdownSetting & ADS119X_CHnSET_PD_MASK ) | (gainSetting   & ADS119X_CHnSET_GAIN_MASK) | (muxSetting    & ADS119X_CHnSET_MUX_MASK)  ; 
 WREG(_address , _valueToWrite);
}

void ADS119X::setAllChannelSettings(byte pwdownSetting, byte gainSetting, byte muxSetting )
{
 for (byte address = ADS119X_ADD_CH1SET; address < ADS119X_ADD_CH1SET +  _num_channels ; address++) {  
   setChannelSettings(address, pwdownSetting, gainSetting, muxSetting ) ; 
 }    
}

void ADS119X::setAllChannelMux(byte muxSetting )
{
 for (byte address = ADS119X_ADD_CH1SET; address < ADS119X_ADD_CH1SET +  _num_channels ; address++) {
   setChannelSettings(address, keepSetting(address) , keepSetting(address), muxSetting ) ;
 } 
   
}

void ADS119X::setAllChannelGain(byte gainSetting )
{
 for (byte address = ADS119X_ADD_CH1SET; address < ADS119X_ADD_CH1SET +  _num_channels ; address++) {
   setChannelSettings(address, keepSetting(address) , gainSetting, keepSetting(address) ) ;
 } 
   
}

byte ADS119X::keepSetting(byte address)
{
   return _regData[address]; 
}


void ADS119X::testNoise()
{
    // Stop continuous conversion
    sendCommand (ADS119X_CMD_SDATAC);
    delay(10);
    // Use Internal Reference (CONFIG3) WREG CONFIG3 0x80
    byte valueToWrite = (_regData[ADS119X_ADD_CONFIG3] & ~ADS119X_REFBUF_MASK) | 0x80 ;
    WREG(ADS119X_ADD_CONFIG3 , valueToWrite);
    
    // Set All Channels to Input Short to test noise
    // REG CHnSET 0x01
    setAllChannelMux(ADS119X_CHnSET_MUX_IN_SHORT);        
    
    //    Send START command if pin Start is tied low in the board
    sendCommand (ADS119X_CMD_START);
    
    // Put the Device Back in RDATAC Mode
    //    send CMD RDATAC
    sendCommand (ADS119X_CMD_RDATAC);
    delay (10);
    
    // Capture Data and Check Noise: Look fot DRDY and issue 24 + n x 16 SCLK
    while (!isDRDY()) {
    }
    readChannelData();
    delay (10);  
}


void ADS119X::testSignal()
{
    // Stop continuous conversion, send SDATAC
    sendCommand (ADS119X_CMD_SDATAC);
    delay(10);
    
    //    Set internal Test Signal, default is (1mV x Vref /2.4)  Square-Wave Test Signal On All Channels      
    //    WREG CONFIG2 0x10  (Set Test signal to internal)
    byte valueToWrite = (_regData[ADS119X_ADD_CONFIG2] & ~ADS119X_INT_TEST_MASK) | 0x10 ;
    WREG(ADS119X_ADD_CONFIG2 , valueToWrite);
    
    //    WREG CHnSET 0x05   (conect channel mux to test/internal/normal/temp signal)
    setAllChannelMux(ADS119X_CHnSET_MUX_TEST); 
    
    //    Put the Device Back in RDATAC Mode, send RDATAC
    sendCommand (ADS119X_CMD_RDATAC);
    delay (10);
    
    //    Capture Data and Test Signal: Look fot DRDY and issue 24 + n x 16 SCLK
    while (!isDRDY()) {
    }
    readChannelData();
    delay (10);
}


void ADS119X::testSignalDC()
{
    // Stop continuous conversion, send SDATAC
    sendCommand (ADS119X_CMD_SDATAC);
    delay(10);
    
    //    Set internal Test Signal, default is (1mV x Vref /2.4)  Square-Wave Test Signal On All Channels      
    //    WREG CONFIG2 0x10  (Set Test signal to internal)
    byte valueToWrite = (_regData[ADS119X_ADD_CONFIG2] & ~ADS119X_INT_TEST_MASK) | ADS119X_INT_TEST ;
    
    valueToWrite = (valueToWrite & ~ADS119X_TEST_FREQ_MASK) | ADS119X_TEST_FREQ_DC ;
    WREG(ADS119X_ADD_CONFIG2 , valueToWrite);
    
    //    WREG CHnSET 0x05   (conect channel mux to test/internal/normal/temp signal)
    setAllChannelMux(ADS119X_CHnSET_MUX_TEST); 
    
    //    Put the Device Back in RDATAC Mode, send RDATAC
    sendCommand (ADS119X_CMD_RDATAC);
    delay (10);
    
    //    Capture Data and Test Signal: Look fot DRDY and issue 24 + n x 16 SCLK
    while (!isDRDY()) {
    }
    readChannelData();
    delay (10);
}


int32_t ADS119X::getStatus()
{
  return _boardStat;
}

int16_t ADS119X::getChannelData(byte channel)
{
    return _channelData[channel];
}

byte ADS119X::getRegisterSize() 
{
  return ADS119X_REG_SIZE;
}
  
byte ADS119X::getRegister(byte address)
{
    return _regData[address];
} 

byte ADS119X::getNumberOfChannels()
{
  return _num_channels;
}

void  ADS119X::setDefaultSettings() {
  // Stop continuous conversion, send SDATAC
  sendCommand (ADS119X_CMD_SDATAC);
  
  byte opcode1 = 0x40; //  WREG expects 010rrrrr where rrrrr = _address
  csLow();                //  open SPI
  xfer(opcode1);                  //  Send WREG command & address
  xfer(ADS119X_REG_SIZE - 1 );    //  Send number of registers to write -1
  for (int i = 0; i <= ADS119X_REG_SIZE - 1 ; i++)
  {
    xfer(_regDataDefault[i]); //  Write to the registers
    _regData[i] = _regDataDefault [i];  // Update local mirror
  }
  csHigh();
     
}

void ADS119X::syncRegData() 
{
     RREGS(ADS119X_ADD_ID, ADS119X_REG_SIZE - 1 );
}

// --------------------------   Private methods


//SPI communication method
byte ADS119X::xfer(byte _data)
{
  byte inByte;
  inByte = SPI.transfer(_data);
  return inByte;
}

//SPI chip select method
void ADS119X::csLow()
{ // select an SPI slave to talk to
    digitalWrite(_cs_pin, LOW);
}

void ADS119X::csHigh()
{ // deselect an SPI slave to talk to
    digitalWrite(_cs_pin, HIGH);
}

void ADS119X::getNumberOfChannelsFromReg ()
{
  byte channelNumReg = _regData [ADS119X_ADD_ID] & ADS119X_DEV_CH_ID_MASK; 
  switch (channelNumReg) {
    case ADS119X_DEV_ID_ADS1194:
      _num_channels = 4 ; 
      break;
    case ADS119X_DEV_ID_ADS1196:
      _num_channels = 6 ; 
      break;
    case ADS119X_DEV_ID_ADS1198:
       _num_channels = 8 ; 
      break;      
    default:
      _num_channels =  4 ; 
      // statements
      break;
  }
}
