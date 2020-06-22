#ifndef ADS1256_h
#define ADS1256_h

#include "Arduino.h"
#include "SPI.h"

#define ADS119X_TOTAL_CH   8   
#define ADS119X_BYTES_PER_CH   2   

//-------------commands (10 in total)
//    System Commands         FIRST BYTE  SECOND BYTE
#define  ADS119X_CMD_WAKEUP    0x02    //  Wake-up from standby mode. NOP command in normal mode.  0000 0010 (02h) 
#define  ADS119X_CMD_STANDBY   0x04    //  Enter standby mode  0000 0100 (04h) 
#define  ADS119X_CMD_RESET   0x06    //  Reset the device  0000 0110 (06h) 
#define  ADS119X_CMD_START   0x08    //  Start/restart (synchronize) conversions 0000 1000 (08h) 
#define  ADS119X_CMD_STOP    0x0A    //  Stop conversion 0000 1010 (0Ah) 
//    Data Read Commands            
#define  ADS119X_CMD_RDATAC    0x10    //  Enable Read Data Continuous mode. This mode is the default mode at power-up. *  0001 0000 (10h) 
#define  ADS119X_CMD_SDATAC    0x11    //  Stop Read Data Continuously mode  0001 0001 (11h) 
#define  ADS119X_CMD_RDATA   0x12    //  Read data by command  supports multiple read back.  0001 0010 (12h) 
//    Register Read Commands        //      
#define  ADS119X_CMD_RREG    0x20    //  Read n nnnn registers starting at address rrrr  **  001r rrrr (2xh) 000n nnnn 
#define  ADS119X_CMD_WREG    0x40    //  Write n nnnn registers starting at address rrrr **  010r rrrr (4xh) 000n nnnn
                
//    * When in RDATAC mode, the RREG command is ignored.           
//    "**  n nnnn = number of registers to be read/written – 1. For example, to read/write three registers, set nnnn = 0 (0010). rrrr = starting register address for read/write opcodes."           

//-------- Register Addreses (26 in total )
#define  ADS119X_REG_SIZE    0x1A  
//    REGISTER        ADDRESS RESET VALUE (Hex) BIT 7 BIT 6 BIT 5 BIT 4 BIT 3 BIT 2 BIT 1 BIT 0
//            Device Settings (Read-Only Registers)                 
#define  ADS119X_ADD_ID    0x00   //  00h XX  DEV_ID5 DEV_ID4 DEV_ID3 1 0 DEV_ID2 DEV_ID1 DEV_ID0
//            Global Settings Across Channels                 
#define  ADS119X_ADD_CONFIG1   0x01   //  01h 4 0 DAISY_EN  CLK_EN  0 0 DR2 DR1 DR0
#define  ADS119X_ADD_CONFIG2   0x02   //  02h 20  0 0 1 INT_TEST  0 TEST_AMP  TEST_FREQ1  TEST_FREQ0
#define  ADS119X_ADD_CONFIG3   0x03   //  03h 40  PD_REFBUF 1 VREF_4V RLD_MEAS  RLDREF_INT  PD_RLD  RLD_LOFF_SENS RLD_STAT
#define  ADS119X_ADD_LOFF    0x04   //  04h 0 COMP_TH2  COMP_TH1  COMP_TH0  VLEAD_OFF_EN  ILEAD_OFF1  ILEAD_OFF0  FLEAD_OFF1  FLEAD_OFF0
//            Channel-Specific Settings                 
#define  ADS119X_ADD_CH1SET    0x05   //  05h 0 PD1 GAIN12  GAIN11  GAIN10  0 MUXn2 MUXn1 MUXn0
#define  ADS119X_ADD_CH2SET    0x06   //  06h 0 PD2 GAIN22  GAIN21  GAIN20  0 MUX22 MUX21 MUX20
#define  ADS119X_ADD_CH3SET    0x07   //  07h 0 PD3 GAIN32  GAIN31  GAIN30  0 MUX32 MUX31 MUX30
#define  ADS119X_ADD_CH4SET    0x08   //  08h 0 PD4 GAIN42  GAIN41  GAIN40  0 MUX42 MUX41 MUX40
#define  ADS119X_ADD_CH5SET   0x09   //  09h 0 PD5 GAIN52  GAIN51  GAIN50  0 MUX52 MUX51 MUX50
#define  ADS119X_ADD_CH6SET   0x0A   //  0Ah 0 PD6 GAIN62  GAIN61  GAIN60  0 MUX62 MUX61 MUX60
#define  ADS119X_ADD_CH7SET   0x0B   //  0Bh 0 PD7 GAIN72  GAIN71  GAIN70  0 MUX72 MUX71 MUX70
#define  ADS119X_ADD_CH8SET   0x0C   //  0Ch 0 PD8 GAIN82  GAIN81  GAIN80  0 MUX82 MUX81 MUX80
#define  ADS119X_ADD_RLD_SENSP   0x0D   //  0Dh 0 RLD8P(1)  RLD7P(1)  RLD6P(1)  RLD5P(1)  RLD4P RLD3P RLD2P RLD1P
#define  ADS119X_ADD_RLD_SENSN   0x0E   //  0Eh 0 RLD8N(1)  RLD7N(1)  RLD6N(1)  RLD5N(1)  RLD4N RLD3N RLD2N RLD1N
#define  ADS119X_ADD_LOFF_SENSP    0x0F   //  0Fh 0 LOFF8P  LOFF7P  LOFF6P  LOFF5P  LOFF4P  LOFF3P  LOFF2P  LOFF1P
#define  ADS119X_ADD_LOFF_SENSN    0x10   //  10h 0 LOFF8N  LOFF7N  LOFF6N  LOFF5N  LOFF4N  LOFF3N  LOFF2N  LOFF1N
#define  ADS119X_ADD_LOFF_FLIP   0x11   //  11h 0 LOFF_FLIP8  LOFF_FLIP7  LOFF_FLIP6  LOFF_FLIP5  LOFF_FLIP4  LOFF_FLIP3  LOFF_FLIP2  LOFF_FLIP1
//            Lead-Off Status Registers (Read-Only Registers)                 
#define  ADS119X_ADD_LOFF_STATP    0x12   //  12h 0 IN8P_OFF  IN7P_OFF  IN6P_OFF  IN5P_OFF  IN4P_OFF  IN3P_OFF  IN2P_OFF  IN1P_OFF
#define  ADS119X_ADD_LOFF_STATN    0x13   //  13h 0 IN8N_OFF  IN7N_OFF  IN6N_OFF  IN5N_OFF  IN4N_OFF  IN3N_OFF  IN2N_OFF  IN1N_OFF
//            GPIO and OTHER Registers                  
#define  ADS119X_ADD_GPIO    0x14   //  14h 0F  GPIOD4  GPIOD3  GPIOD2  GPIOD1  GPIOC4  GPIOC3  GPIOC2  GPIOC1
#define  ADS119X_ADD_PACE    0x15   //  15h 0 0 0 0 PACEE1  PACEE0  PACEO1  PACEO0  PD_PACE
#define  ADS119X_ADD_RESERVED    0x16   //  16h 0 0 0 0 0 0 0 0 0
#define  ADS119X_ADD_CONFIG4   0x17   //  17h 0 0 0 0 0 SINGLE_SHOT WCT_TO_RLD  PD_LOFF_COMP  0
#define  ADS119X_ADD_WCT1    0x18   //  18h 0 aVF_CH6 aVL_CH5 aVR_CH7 avR_CH4 PD_WCTA WCTA2 WCTA1 WCTA0
#define  ADS119X_ADD_WCT2    0x19   //  19h 0 PD_WCTC PD_WCTB WCTB2 WCTB1 WCTB0 WCTC2 WCTC1 WCTC0


//-------- Device Settings ID Constant Values (Read Only)
#define ADS119X_ID_7_2           0xB4    // 101x xxxx is ADS119X device family, xxx1 01xx this part is constant read only

#define ADS119X_DEV_CH_ID_MASK   0x03    // xxxx xxnn nn are the ID of channels
#define ADS119X_DEV_ID_ADS1194   0x00    // xxxx xx00 is ADS1194
#define ADS119X_DEV_ID_ADS1196   0x01    // xxxx xx01 is ADS1196
#define ADS119X_DEV_ID_ADS1198   0x02    // xxxx xx10 is ADS1198

//-------- Config 1
#define ADS119X_DAISY_MASK    0x40   //
#define ADS119X_DAISY_ENABLE  0x00   //
#define ADS119X_DAISY_DISABLE 0x40   //

#define ADS119X_CLK_MASK    0x20   //
#define ADS119X_CLK_DISABLE 0x00   //
#define ADS119X_CLK_ENABLE  0x20   //

#define ADS119X_DRATE_MASK    0x07
#define ADS119X_DRATE_8000SPS 0x00
#define ADS119X_DRATE_4000SPS 0x01
#define ADS119X_DRATE_2000SPS 0x02
#define ADS119X_DRATE_1000SPS 0x03
#define ADS119X_DRATE_500SPS  0x04
#define ADS119X_DRATE_250SPS  0x05
#define ADS119X_DRATE_125SPS  0x06


//-------- Config 2
#define ADS119X_INT_TEST_MASK   0x10
#define ADS119X_EXT_TEST        0x00
#define ADS119X_INT_TEST        0x10

#define ADS119X_TEST_AMP_MASK   0x04
#define ADS119X_TEST_AMP_X1     0x00
#define ADS119X_TEST_AMP_X2     0x04

#define ADS119X_TEST_FREQ_MASK  0x03
#define ADS119X_TEST_FREQ_21    0x00
#define ADS119X_TEST_FREQ_20    0x01
#define ADS119X_TEST_FREQ_DC    0x03

//-------- Config 3
#define ADS119X_REFBUF_MASK            0x80
#define ADS119X_VREF_4V_MASK           0x20
#define ADS119X_RLD_MEAS_MASK          0x10
#define ADS119X_RLDREF_INT_MASK        0x08
#define ADS119X_NOT_PD_RLD_MASK        0x04
#define ADS119X_RLD_LOFF_SENS_MASK     0x02
#define ADS119X_RLD_STAT_MASK          0x01

//-------- LOFF


//-------- CHnSET
#define ADS119X_CHnSET_PD_MASK        0x80

#define ADS119X_CHnSET_GAIN_MASK      0x70
#define ADS119X_CHnSET_GAIN_6         0x00
#define ADS119X_CHnSET_GAIN_1         0x10
#define ADS119X_CHnSET_GAIN_2         0x20
#define ADS119X_CHnSET_GAIN_3         0x30
#define ADS119X_CHnSET_GAIN_4         0x40
#define ADS119X_CHnSET_GAIN_8         0x50
#define ADS119X_CHnSET_GAIN_12        0x60

#define ADS119X_CHnSET_MUX_MASK       0x07
#define ADS119X_CHnSET_MUX_NORMAL     0x00
#define ADS119X_CHnSET_MUX_IN_SHORT   0x01
#define ADS119X_CHnSET_MUX_RLD_MEAS   0x02
#define ADS119X_CHnSET_MUX_MVDD       0x03
#define ADS119X_CHnSET_MUX_TEMP       0x04
#define ADS119X_CHnSET_MUX_TEST       0x05
#define ADS119X_CHnSET_MUX_RLD_DRP    0x06
#define ADS119X_CHnSET_MUX_RLD_DRN    0x07

//-------- RLD_SENSP

//-------- RLD_SENSN

//-------- LOFF_SENSP

//-------- LOFF_SENSN

//-------- LOFF_FLIP

//-------- LOFF_STATP (Read-Only Register)

//-------- LOFF_STATN (Read-Only Register)

//-------- GPIO: General-Purpose I/O Register
#define ADS119X_GPIO_DATA_MASK       0x70

#define ADS119X_GPIO_CONTROL_MASK       0x70

//-------- PACE: PACE Detect Register
#define ADS119X_PACEE_MASK              0x18
#define ADS119X_PACEO_MASK              0x06
#define ADS119X_NOT_PD_PACE_MASK        0x01


//-------- CONFIG4: Configuration Register 4
#define ADS119X_SINGLE_SHOT_MASK            0x08
#define ADS119X_WCT_TO_RLD_MASK             0x08
#define ADS119X_NOT_PD_LOFF_COMP_MASK       0x08

//-------- WCT1

//-------- WCT2





class ADS119X {
 public:
  ADS119X(byte dataReady_Pin, byte reset_Pin, byte cs_Pin);  
  boolean begin();    

  void sendCommand(unsigned char cmd);
  void WREG(byte address, byte value);
  byte RREG(byte address);
  void RREGS(byte  address, byte  numRegistersMinusOne); 

  void setDataRate(byte dataRate); 
  void readChannelData(); 
  
  boolean isDRDY();
  
  void setChannelSettings(byte address, byte pwdownSetting, byte gainSetting, byte muxSetting )  ;
  void setAllChannelSettings(byte pwdownSetting, byte gainSetting, byte muxSetting );
  void setAllChannelMux(byte muxSetting );
  void setAllChannelGain(byte gainSetting );  

  byte keepSetting(byte address);
  
  void testNoise();
  void testSignal();
  void testSignalDC();

  int32_t getStatus();
  int16_t getChannelData(byte channel);  

  byte getRegisterSize(); 
  byte getRegister(byte address); 

  byte getNumberOfChannels();
  
  void  setDefaultSettings();   

  void  syncRegData();   
   
  
 private:
  byte xfer(byte _data); 
  void csLow();
  void csHigh();
  void getNumberOfChannelsFromReg ();
  
  // pins 
  byte _drdy_pin;
  byte _reset_pin;
  byte _cs_pin ; 
    
  //-------- Lib Copy of Regs and Settings 
  byte _regData[ADS119X_REG_SIZE] ; 
  byte _regDataDefault[ADS119X_REG_SIZE] ; 
  byte _num_channels ;
  int16_t _channelData[ADS119X_TOTAL_CH];    
  uint32_t _boardStat;
  
};

#endif
