byte bytebuffer[32] = {255, 255, 170, 85, 168, 1, 67, 102, 188, 179, 0, 176, 35, 247, 15, 247, 15, 238, 0, 31, 232, 0, 18, 231, 129, 71, 220, 129, 160, 102, 188, 179};
byte syncpacket[4] = {255, 255, 170, 85};
byte secondmask = 15;
byte firstnibble;
byte secondnibble;
int j;
unsigned short twobytestore; 
int stored = 0; // keep track of the number of bits stored.
int arrayloc = 0;
int lastsynch;

// accelerometer vars - HAVE
  unsigned short accch1;
  unsigned short accch2;
  unsigned short accch3;
  int bitsforaccch1[10];
  int bitsforaccch2[10];
  int bitsforaccch3[10];
  byte accheaderbytes[3]; // 1 to 3 bytes is the length of the header. Based on whether there are dropped packets.
  byte accdatabytes[4];

//uncompressed EEG vars - HAVE
  unsigned short ueegch1;
  unsigned short ueegch2;
  unsigned short ueegch3;
  unsigned short ueegch4;
  int bitsforueegch1[10];
  int bitsforueegch2[10];
  int bitsforueegch3[10];
  int bitsforueegch4[10];
  byte ueegheaderbytes[3]; // 1 to 3 bytes is the length of the header. Based on whether there are dropped packets.
  byte ueegdatabytes[5]; // 5 byte data payload in 10 bit groups

// error flags vars -- probably won't need to deal with
  int bitsforerr[32];
  byte errdatabytes[4];

//Compressed EEG vars
  // for final calculated values
  unsigned short calceegch1; 
  unsigned short calceegch2;
  unsigned short calceegch3;
  unsigned short calceegch4;
  
  byte ceegheaderbytes[1]; // header byte - just the first 4 bits are used
  
  byte ceegmqbytes[5]; // meadians and quantization bytes
  int bitsforceegmqch1[10]; // each of these is 4 bits for quantization and 6 bits for median. so I'll need 1-byte values for each of those.
    byte ceegqch1; // 4 rightmost bits will be data
    byte ceegmch1; // 6 rightmost bits will be data
  int bitsforceegmqch2[10];
    byte ceegqch2; // 4 rightmost bits will be data
    byte ceegmch2; // 6 rightmost bits will be data
  int bitsforceegmqch3[10];
    byte ceegqch3; // 4 rightmost bits will be data
    byte ceegmch3; // 6 rightmost bits will be data
  int bitsforceegmqch4[10];
    byte ceegqch4; // 4 rightmost bits will be data
    byte ceegmch4; // 6 rightmost bits will be data
    
  byte ceegbitlengthbytes[2]; //bit length bytes - doesn't seem to be used
  
  // data for this is really confusing so I'll need to go back to that. 


// Battery vars -- HAVE
  unsigned short batpercent; // 2 bytes
  unsigned short fuelgaugemv; // 2 bytes
  unsigned short adcmv; // 2 bytes
  unsigned short tempc; // 2 bytes
  // can just write to the bytes because it's nice and even! like in errors
  byte batheadbytes[1]; // header is 1 byte
  byte batdatabytes[8]; // payload is 8 bytes

// 9 DRL / REF vars
  /*
  If data is compressed this packet is transmitted, 
  which contains the DRL and REF electrode reading.
  This happens on a smaller frequency than regular 
  EEG channels and is used to determine if the Muse
  is touching the forehead or not.
  */
  unsigned short drl; // 10-bit DRL reading
  unsigned short ref; // 10-bit REF reading
  int bitsfordrl[10];
  int bitsforref[10];
  byte drlrefheaderbytes[1];
  byte drlrefdatabytes[3];

         
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
       Serial.print("\t THIS IS A SYNCHPACKET! \n");
       // Now I know where a sync packet is. Can I build the rest from it? Maybe first with just the order I know? will need to make something to more programattically do this though.
       // maybe I should save where this is (the I value) so I can go back to it?
       lastsynch = i;
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
         
          twobytestore = writebytestoshort(bytebuffer[i+j], bytebuffer[i+j+1]);
          ++j;
          Serial.print("\t 2 byte value: ");
          Serial.println(twobytestore); // this number is right!
          //Serial.print ("bits of twobytestore: ");
          ++j; // now I should be able to keep using i+j, and then just add j to the end to move on.          
         }
         //4 byte payload. ACC Ch1: 10bits / ACC CH2: 10bits / ACC CH3 10bits / 2 unused bits.
         // so store it in unsigned short variables. I think I need to add the 0's at the beginning. So first maybe just pop the bin values in arrays?
         for(int a=0; a<4; ++a){
           accdatabytes[a] = bytebuffer[i+j]; // add this byte
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
         arrayloc = 15;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(accch1, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(accch1, d, bitsforaccch1[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               --arrayloc;
             }
          }
         Serial.print("accch1: "); Serial.println(accch1);
           
         // write the second channel into unsigned short accch2
         //Serial.print("Writing into the unsigned short accch2\n");
         arrayloc = 15;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(accch2, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(accch2, d, bitsforaccch2[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               --arrayloc;
             }
          }
         Serial.print("accch2: "); Serial.println(accch2);
         
         // write the third channel into unsigned short accch3
         //Serial.print("Writing into the unsigned short accch3\n");
         arrayloc = 15;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(accch3, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(accch3, d, bitsforaccch3[arrayloc]);
               //Serial.println(bitsforaccch3[arrayloc]);
               --arrayloc;
             }
          }
         Serial.print("accch3: "); Serial.println(accch3);

         // move the counter past bytes we've read. Must have already upped it one.
         i = i+j-1;
         break;  
       case 14:
         // E - uncompressed EEG
         j = 1; // this is the offset to add to i at the end of this part of the loop
         Serial.println("\t Uncompressed EEG packet");  
         if(secondnibble == 8) {
           Serial.println("\t samples have been dropped."); 
           // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
           // make sure I'm writing this in the right order? - I think so!
           twobytestore = writebytestoshort(bytebuffer[i+j], bytebuffer[i+j+1]);
          ++j;
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
         
         for(int a=0; a<5; ++a){
           ueegdatabytes[a] = bytebuffer[i+j]; // add this byte
           Serial.print("Adding byte: "); Serial.print(i+j); Serial.print(" as the "); Serial.print(a); Serial.println("data byte");
           ++j; // move the counter for the next one.
         }
         Serial.println("\t Uncompressed EEG data bytes:"); 
         for(int a=0; a<5; ++a){
           Serial.print("\t\tByte "); Serial.print(a); Serial.print(": ");
           Serial.println(ueegdatabytes[a]); 
         }
         
        // write the bits into arrays of 10
         stored = 0; // keep track of the number of bits stored.
         for(int a=0; a<5; ++a){
          //working with each byte.
          //Serial.print("\t uncompressed eeg byte "); Serial.print(a); Serial.println(": ");
          for(int b=0; b<8; ++b){
            int thisbit = bitRead(ueegdatabytes[a], b);
            //Serial.print("\t\t bit "); Serial.print(b); Serial.print(": "); Serial.println(thisbit);
            if(stored<10){
              if(stored==0){Serial.print("\nChannel 1: ");}
              bitsforueegch1[stored]= thisbit;
              ++stored;
               Serial.print(thisbit);
            }else if(stored>=10 && stored<20){
              if(stored==0){Serial.print("\nChannel 2: ");}
              bitsforueegch2[stored-10]=thisbit;
              ++stored;
            }else if(stored>=20 && stored<30){
              if(stored==0){Serial.print("\nChannel 3: ");}
              bitsforueegch3[stored-20]= thisbit;
              ++stored;
            }else if(stored>=30 && stored<40){
              if(stored==0){Serial.print("\nChannel 4: ");}
              bitsforueegch4[stored-30]= thisbit;
              ++stored;
            }else{
              //Serial.print("\tUnused Bit: "); Serial.println(thisbit);              
            }
          }         
         }
         
         // write the first channel into unsigned short ueegch1
         //Serial.print("Writing into the unsigned short ueegch1\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(ueegch1, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(ueegch1, d, bitsforueegch1[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("ueegch1: "); Serial.println(ueegch1);     //correct somehow.
       
         // write the first channel into unsigned short ueegch1
         //Serial.print("Writing into the unsigned short ueegch1\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(ueegch2, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(ueegch2, d, bitsforueegch2[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("ueegch2: "); Serial.println(ueegch2);  //incorrect
         
         // write the first channel into unsigned short ueegch1
         //Serial.print("Writing into the unsigned short ueegch1\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(ueegch3, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(ueegch3, d, bitsforueegch3[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("ueegch3: "); Serial.println(ueegch3); // incorrect
         
         // write the first channel into unsigned short ueegch1
         //Serial.print("Writing into the unsigned short ueegch1\n");
         arrayloc = 0;
         for(int d=15; d>=0; --d){
             //Serial.print(d); Serial.print(": ");
             if(d<16 && d>9){
               bitWrite(ueegch4, d, 0);
               //Serial.println("0");
             }else{
               bitWrite(ueegch4, d, bitsforueegch4[arrayloc]);
               //Serial.println(bitsforaccch1[arrayloc]);
               ++arrayloc;
             }
          }
         Serial.print("ueegch4: "); Serial.println(ueegch4); //incorrect
         
         
         i = i+j-1;
         break;  
       case 13:
         // D - error flags 
         Serial.println("\t Error Flags packet");    
         break;
       case 12:
         // C - Compressed EEG
         Serial.println("\t Compressed EEG packet");  
          j = 1; // this is the offset to add to i at the end of this part of the loop
         if(secondnibble == 8) {
           Serial.println("\t samples have been dropped."); 
           // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
                      // need to get the next 2 bytes and write them into a 2-byte (16 bit) short number: "If this flag is set, a two-byte number (short) is concatenated to the header. 
           //The two extra bytes specify numbers of samples dropped by the Muse since the last successful package of same type. Currently only EEG and Accelerometer samples 
           //transmit information about dropped samples, as the rest of the data types are less critical.
           // make sure I'm writing this in the right order? - I think so!
           twobytestore = writebytestoshort(bytebuffer[i+j], bytebuffer[i+j+1]);
          ++j;
          Serial.print("\t 2 byte value: ");
          Serial.println(twobytestore);
          ++j; // now I should be able to keep using i+j, and then just add j to the end to move on.       
         }
         // will need to do more stuff with this one.
         i = i+j-1;
         break;  
       case 11: 
         // B - Battery
          j = 1; // this is the offset to add to i at the end of this part of the loop
         Serial.println("\t Battery packet");  
         // has no second nibble
         for(int a=0; a<8; ++a){
           batdatabytes[a] = bytebuffer[i+j]; // add this byte
           Serial.print("Adding byte: "); Serial.print(i+j); Serial.print(" as the "); Serial.print(a); Serial.println("data byte");
           ++j; // move the counter for the next one.
         }
         Serial.println("\t Battery data bytes:"); 
         for(int a=0; a<8; ++a){
           Serial.print("\t\tByte "); Serial.print(a); Serial.print(": ");
           Serial.println(batdatabytes[a]); 
         }
         /* the order of the data bytes is:
         batdatabytes[0], batdatabytes[1] = battery percent, unsigned short batpercent; // 2 bytes
         batdatabytes[2], batdatabytes[3] = fuel millivolts, unsigned short fuelgaugemv; // 2 bytes
         batdatabytes[4], batdatabytes[5] = ADC millivolts, unsigned short fuelgaugemv; // 2 bytes
         batdatabytes[6], batdatabytes[7] = Temperature in C, unsigned short fuelgaugemv; // 2 bytes
         */
         batpercent = writebytestoshort(batdatabytes[0], batdatabytes[2]);
         fuelgaugemv = writebytestoshort(batdatabytes[2], batdatabytes[3]);
         adcmv = writebytestoshort(batdatabytes[4], batdatabytes[5]);
         tempc = writebytestoshort(batdatabytes[6], batdatabytes[7]);
         
         Serial.print("Battery Percent: "); Serial.println(batpercent);
         Serial.print("Fuel MV: "); Serial.println(fuelgaugemv);
         Serial.print("ADC MV: "); Serial.println(adcmv);
         Serial.print("Temp in C: "); Serial.println(tempc);
         // ++j; // now I should be able to keep using i+j, and then just add j to the end to move on.          
         
         i = i+j-1;
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




