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

void print16bits(int var) {
  for (unsigned int bitpos = 0x8000; bitpos; bitpos >>= 1) {
    Serial.write(var  & bitpos ? '1' : '0');
  }
  Serial.println();
}

void print24bits(int var) {
  for (unsigned int bitpos = 0x800000; bitpos; bitpos >>= 1) {
    Serial.write(var  & bitpos ? '1' : '0');
  }
  Serial.println();
}

void print32bits(int var) {
  for (unsigned int bitpos = 0x80000000; bitpos; bitpos >>= 1) {
    Serial.write(var  & bitpos ? '1' : '0');
  }
  Serial.println();
}
