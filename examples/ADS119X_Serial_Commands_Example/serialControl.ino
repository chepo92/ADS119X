//============

void recvWithEndMarker() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    //char startMarker = '<';
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index
    
    strtokIndx = strtok(tempChars,",");      // 
    for(uint8_t i = 0 ; i  < 2 ; i++ ){       
        command[i] = atoi(strtokIndx);     // convert this part to an integer
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    }            

}

//============

void showParsedData() {
    Serial.println("Commands: ");
    for(uint8_t i = 0 ; i  < 2 ; i++ ){       
        Serial.print (command[i]);   
        Serial.print(", ");
    }   
    
    Serial.println("");

}
