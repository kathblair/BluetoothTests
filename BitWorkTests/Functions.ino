unsigned short writebytestoshort(byte byte1, byte byte2){
  int writeloc = 15;
  unsigned short target;
  for(int a=7; a>=0; --a){
    int thisbit = bitRead(byte1, a); // first byte of the unsigned short
    //Serial.print("bit "); Serial.print(a); Serial.print(": "); Serial.println(thisbit);
    bitWrite(target, writeloc, thisbit);
    --writeloc;
  }
  for(int a=7; a>=0; --a){
    int thisbit = bitRead(byte2, a); // second byte of sample packet
    //Serial.print("bit "); Serial.print(a); Serial.print(": "); Serial.println(thisbit);
    bitWrite(target, writeloc, thisbit);
    --writeloc;
  } 
  return target;
}

// I want this function to basically figure out what packet I'm in. So I guess I need to start with the first 
char buildpacket(byte buffer, int i){
  //get the first nibble of the first byte
  //byte fnibble = buffer[i] << 4;
}
