unsigned short writebytestoshort(byte byte1, byte byte2){
  int writeloc = 15;
  unsigned short target;
  for(int a=7; a>=0; --a){
    int thisbit = bitRead(byte1, a); // first byte of the unsigned short
    //Serial.print("writeloc: "); Serial.print(writeloc); Serial.print("| bit "); Serial.print(a); Serial.print(": "); Serial.println(thisbit);
    bitWrite(target, writeloc, thisbit);
    --writeloc;
  }
  for(int a=7; a>=0; --a){
    int thisbit = bitRead(byte2, a); // second byte of sample packet
    //Serial.print("writeloc: "); Serial.print(writeloc); Serial.print("| bit "); Serial.print(a); Serial.print(": "); Serial.println(thisbit);
    bitWrite(target, writeloc, thisbit);
    --writeloc;
  } 
  return target;
}

unsigned short tenbitstotwobytes(int bitarray[10]){
  // write the first channel into unsigned short accch1
  unsigned short target;
  //Serial.print("Writing into the unsigned short ");
  arrayloc = 0; // STARTING FROM THE BEGINNING
  for(int d=15; d>=0; --d){ // d is the locaiton in the bits to write
    //Serial.print(d); Serial.print(": ");
    if(d<16 && d>9){
      bitWrite(target, d, 0);
    }else{
      bitWrite(target, d, bitarray[arrayloc]);
      arrayloc++;
    }
    //Serial.print(" ");
  }
  //Serial.println();
  //Serial.print("target: "); Serial.println(target);
  return target;
}



// I want this function to basically figure out what packet I'm in. So I guess I need to start with the first 
char buildpacket(byte buffer, int i){
  //get the first nibble of the first byte
  //byte fnibble = buffer[i] << 4;
}
