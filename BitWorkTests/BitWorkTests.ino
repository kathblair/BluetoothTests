byte bytebuffer[32] = {255, 255, 170, 85, 168, 1, 67, 102, 188, 179, 0, 176, 35, 247, 15, 247, 15, 238, 0, 31, 232, 0, 18, 231, 129, 71, 220, 129, 160, 102, 188, 179};
byte syncpacket[4] = {255, 255, 170, 85};
byte secondmask = 15;
byte firstnibble;
byte secondnibble;
int j;
unsigned short twobytestore; 
int stored = 0; // keep track of the number of bits stored.
int arrayloc = 0;

// accelerometer vars
unsigned short accch1;
unsigned short accch2;
unsigned short accch3;
int bitsforaccch1[10];
int bitsforaccch2[10];
int bitsforaccch3[10];
byte accdatabytes[4];

         
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i;
  for (i=0; i < sizeof(bytebuffer); i = i+1) {
    Serial.print("byte");
    Serial.print(i);
    Serial.print(": ");
    //so each of these should be 8 bits. 
    Serial.print(bytebuffer[i], HEX);
    Serial.print("(HEX) / ");
    Serial.print(bytebuffer[i], DEC);
    Serial.print("(DEC) / ");
    Serial.print(bytebuffer[i], BIN);
    Serial.print("(BIN)\r\n");
    
    if(bytebuffer[i] == syncpacket[0] && bytebuffer[i+1] == syncpacket[1] && bytebuffer[i+2] == syncpacket[2] && bytebuffer[i+3] == syncpacket[3] ){
       Serial.print("\t\THIS IS A SYNCHPACKET! \n");
       // Now I know where a sync packet is. Can I build the rest from it? Maybe first with just the order I know? will need to make something to more programattically do this though.
        i=i+3; // skip to the next item, knowing we're done syncing?
    }else{
     // THIS WORKS YAYYYYYY
     byte firstnibble =  bytebuffer[i] >> 4;
     Serial.print("\tFirstnibble: ");
     Serial.print(firstnibble, HEX);
     Serial.print("(HEX)| ");
     Serial.print(firstnibble, DEC);
     Serial.print("(BIN)\r\n");
     
     byte secondnibble =  bytebuffer[i] & secondmask;
     Serial.print("\tSecondnibble: ");
     Serial.print(secondnibble, HEX);
     Serial.print("(HEX) | ");
     Serial.print(secondnibble, DEC);
     Serial.print("(BIN)\r\n");    
     
     // next thing to do is build the insructions for reading each packet, and maybe make this also look for the sync packet.
     // ord do I find the sync packet first like I'm doing, and then start reading stuff? 
     switch(firstnibble){
       case 10:
         // A - Accelerometer
         j = 1; // this is the offset to add to i at the end of this part of the loop
         Serial.println("\t Accelerometer packet"); 
         if(secondnibble == 8) {
           Serial.println("\t samples have been dropped."); 
           // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
           // make sure I'm writing this in the right order? - I think so!
           int writeloc = 15;
              //Serial.print("Byte that is first of dropped sample int: ");
              //Serial.println(i+j);
           for(int a=0; a<8; ++a){
              int thisbit = bitRead(bytebuffer[i+j], a); // first byte of dropped sample packet
              bitWrite(twobytestore, writeloc, thisbit);
             --writeloc;
          }
          ++j; // move to next byte of sample packet
          //Serial.println(i+j);
           for(int a=0; a<8; ++a){
              int thisbit = bitRead(bytebuffer[i+j], a); // second byte of sample packet
              bitWrite(twobytestore, writeloc, thisbit);
              --writeloc;
          }
          Serial.print("\t 2 byte value: ");
          Serial.println(twobytestore);
          ++j; // now I should be able to keep using i+j, and then just add j to the end to move on.          
         }
         //4 byte payload. ACC Ch1: 10bits / ACC CH2: 10bits / ACC CH3 10bits / 2 unused bits.
         // so store it in unsigned short variables. I think I need to add the 0's at the beginning. So first maybe just pop the bin values in arrays?
         for(int a=0; a<4; ++a){
           accdatabytes[a] = bytebuffer[i+j]; // ad this byte
           Serial.print("Adding byte: "); Serial.print(i+j); Serial.print(" as the "); Serial.print(a); Serial.println("data byte");
           ++j; // move the counter for the next one.
         }
         Serial.println("\t Accelerometer data bytes:"); 
         for(int a=0; a<4; ++a){
           Serial.print("\t\tByte "); Serial.print(a); Serial.print(": ");
           Serial.println(accdatabytes[a]); 
         }
         // now I've gotta break it into 10-bit chucks.
         stored = 0; // keep track of the number of bits stored.
         for(int a=0; a<4; ++a){
          //working with each byte.
          //Serial.print("\t Accelerometer byte "); Serial.print(a); Serial.println(": ");
          for(int b=0; b<8; ++b){
            int thisbit = bitRead(accdatabytes[a], b);
            //Serial.print("\t\t bit "); Serial.print(b); Serial.print(": "); Serial.println(thisbit);
            if(stored<10){
              bitsforaccch1[stored]= thisbit;
              ++stored;
            }else if(stored>=10 && stored<20){
              bitsforaccch2[stored-10]=thisbit;
              ++stored;
            }else if(stored>=20 && stored<30){
              bitsforaccch3[stored-20]= thisbit;
              ++stored;
            }else{
              //Serial.print("\tUnused Bit: "); Serial.println(thisbit);              
            }
          }         
         }
         // write the first channel into unsigned short accch1
         //Serial.print("Writing into the unsigned short accch1\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(accch1, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(accch1, d, bitsforaccch1[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("accch1: "); Serial.println(accch1);
         
         
         
         // write the second channel into unsigned short accch2
         //Serial.print("Writing into the unsigned short accch2\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(accch2, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(accch2, d, bitsforaccch2[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("accch2: "); Serial.println(accch2);
         
         
         // write the third channel into unsigned short accch3
         //Serial.print("Writing into the unsigned short accch3\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(accch3, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(accch3, d, bitsforaccch3[arrayloc]);
               //Serial.println(bitsforaccch3[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("accch3: "); Serial.println(accch3);

         // move the counter past bytes we've read. Must have already upped it one.
         i = i+j-1;
         break;  
       case 14:
         // E - uncompressed EEG
         Serial.println("\t Uncompressed EEG packet");  
         if(secondnibble == 8) {
           Serial.println("\t samples have been dropped."); 
           // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
           // make sure I'm writing this in the right order? - I think so!
           int writeloc = 15;
           for(int a=0; a<8; ++a){
              int thisbit = bitRead(bytebuffer[i+j], a); // first byte of dropped sample packet
              bitWrite(twobytestore, writeloc, thisbit);
             --writeloc;
          }
          ++j; // move to next byte of sample packet
           for(int a=0; a<8; ++a){
              bitWrite(twobytestore, writeloc, a);
              int thisbit = bitRead(bytebuffer[i+j], a); // second byte of sample packet
              bitWrite(twobytestore, writeloc, thisbit);
              --writeloc;
          }
          Serial.print("\t 2 byte value: ");
          Serial.println(twobytestore);
          ++j; // now I should be able to keep using i+j, and then just add j to the end to move on.       
         }
         // get the next 5 bytes and rewrite into different infos. There are 4 channels with 10 bit sample width each. The order is the one specified in eegChannelLayout in the status output.
         // EEG channel 1: 10 bits
         // EEG channel 2: 10 bits
         // EEG channel 3: 10 bits
         // EEG channel 4: 10 bits
         // that's exacly 5 bytes.
         i = i+j-1;
         break;  
       case 13:
         // D - error flags 
         Serial.println("\t Error Flags packet");    
         break;
       case 12:
         // C - Compressed EEG
         Serial.println("\t Compressed EEG packet");  
         if(secondnibble == 8) {
           Serial.println("\t samples have been dropped."); 
           // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
                      // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
           // make sure I'm writing this in the right order? - I think so!
           int writeloc = 15;
              //Serial.print("Byte that is first of dropped sample int: ");
              //Serial.println(i+j);
           for(int a=0; a<8; ++a){
              int thisbit = bitRead(bytebuffer[i+j], a); // first byte of dropped sample packet
              bitWrite(twobytestore, writeloc, thisbit);
             --writeloc;
          }
          ++j; // move to next byte of sample packet
          //Serial.println(i+j);
           for(int a=0; a<8; ++a){
              int thisbit = bitRead(bytebuffer[i+j], a); // second byte of sample packet
              bitWrite(twobytestore, writeloc, thisbit);
              --writeloc;
          }
          Serial.print("\t 2 byte value: ");
          Serial.println(twobytestore);
          ++j; // now I should be able to keep using i+j, and then just add j to the end to move on.       
         }
         // will need to do more stuff with this one.
         i = i+j-1;
         break;  
       case 11: 
         // B - Battery
         Serial.println("\t Battery packet");  
         break;  
       case 9:
         // 9 DRL / REF
         Serial.println("\t DRL / REF packet"); 
         //If data is compressed this packet is transmitted, which contains the DRL and REF electrode reading. This happens on a smaller frequency than regular EEG channels 
         //and is used to determine if the Muse is touching the forehead or not.
         //Payload is 10-bit DRL reading and 10-bit REF reading.
         //1 byte header, 3 byte payload
         break;  
       case 0: 
         // 0 Invalid
         Serial.println("\t Invalid packet");    
         break;
     }
    }
    /*for(int a=0; a<8; ++a){
      Serial.print("\tbit");
      Serial.print(a);
      Serial.print(": ");
      Serial.println(bitRead(bytebuffer[i], a));
    }*/
    delay(2000);
  }
//delay(5000);
}

// I want this function to basically figure out what packet I'm in. So I guess I need to start with the first 
char buildpacket(byte buffer, int i){
  //get the first nibble of the first byte
  //byte fnibble = buffer[i] << 4;
}


