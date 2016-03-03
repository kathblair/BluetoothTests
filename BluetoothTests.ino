/*
  Copyright (c) 2014 MediaTek Inc.  All right reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License..
 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
   See the GNU Lesser General Public License for more details.
*/

/*

Notes:


- want to make it so it does search, just connects. 
- will nede to figure out my timing etc.


TESTING: just connect, doesn't have to be broadcasting.

*/
#include <LBT.h>
#include <LBTClient.h>
#include <LBTServer.h>
#include <LWs2812.h>
 
static LBTDeviceInfo info = {0};
boolean find = 0;
#define SPP_SVR "Muse-F11F" // it should be the prefer server's name,  customize it yourself.
#define ard_log Serial.printf
#define SPP_ADDR "00:06:66:77:f1:1f" // MAC Address of the tartget device. 
int read_size = 0;
void setup()  
{
  find = 1;
  Serial.begin(9600);
  ard_log("LBT start\n");
  Serial1.print("Ws2812 test.\r\n");
  LWs2812.check_on_line();
  // begin BT
  bool success = LBTClient.begin();
  if( !success )
  {
      ard_log("Cannot begin Bluetooth Client successfully\n");
      delay(0xffffffff);
  }
  else
  {
      ard_log("Bluetooth Client begin successfully\n");
      // maybe I try to connect in setup?
  }
}
 
void loop()
{
   
    // to check if the connection is ready
    if(find && !LBTClient.connected())
    {
        ard_log("Diconnected, try to connect\n");
        // do connect
        bool conn_result = LBTClient.connect(SPP_ADDR); // was info.address
        ard_log("connect result [%d]", conn_result);
 
        if( !conn_result )
        {
            ard_log("Cannot connect to SPP Server successfully \r\n");
            delay(0xffffffff);
        }
        else
        {
            ard_log("Connect to SPP Server successfully \r\n");
            LWs2812.monochrome(5, 0x00ff0000, 2000);
            delay(2500);
        }

    }
    
    if(LBTClient.connected()){
      ard_log("Connected!\r\n");     
      char buffer[91] = {0};
      byte bytebuffer[32] = {0};
      int write_size2 = LBTClient.write("r 2\r\n", strlen( "r 2\r\n" )); //Android
      ard_log("write_size: [%d] \r\n", write_size2);
      int write_size = LBTClient.write("% 12\r\n", strlen( "% 12\r\n" )); //Set to Mode 12
      ard_log("client first spec write_size [%d] \r\n", write_size);
      int write_size3 = LBTClient.write("s\r\n", strlen( "s\r\n" )); //Start streaming data and we'll see what we get!
      ard_log("client first spec write_size [%d] \r\n", write_size3);
      //Will need to have a timer to send keep alive every 5-10 seconds ("k\r\n");
      //int write_size = LBTClient.write("v 2\r\n", strlen( "v 2\r\n" ));
      //ard_log("client first spec write_size [%d]", write_size);
           
       // should send data back about the version, etc.
       while(1)
        {
           if(LBTClient.available())
           {
           read_size = LBTClient.readBytes(bytebuffer,32);
           break;
           }
           delay(100);
        }
        ard_log("read size [%d][%s] \r\n", read_size, bytebuffer);
        int i;
        for (i=0; i < sizeof(bytebuffer); i = i+1) {
          Serial.print("byte");
          Serial.print(i);
          Serial.print(": ");
          //so each of these should be 8 bits. 
          Serial.println(bytebuffer[i], BIN);
        }
        
 
      if(LBTClient.available())
        {           
           
           //int write_size2 = LBTClient.write("r 2\r\n", strlen( "r 2\r\n" )); //Android
           //ard_log("client first spec write_size [%d]", write_size2);
           // should set the mode to EEG + Acc, should not return anything but we'll give it a shot:
           //read_size = LBTClient.readBytes(buffer,32);
           //ard_log("read size [%d][%s] \r\n", read_size, buffer);
           
           //int write_size3 = LBTClient.write("% 12\r\n", strlen( "% 12\r\n" )); //EEG+Acc
           //ard_log("client first spec write_size [%d]", write_size3);
           //read_size = LBTClient.readBytes(buffer,32);
           //ard_log("read size [%d][%s] \r\n", read_size, buffer);
           
           
           //int write_size4 = LBTClient.write("s\r\n", strlen( "s\r\n" )); //Start streaming data
           //ard_log("client first spec write_size [%d]", write_size4); //Start streaming data
           //read_size = LBTClient.readBytes(buffer,32);
           //ard_log("read size [%d][%s] \r\n", read_size, buffer);
           
           // at some point I'll want to stop sending data 
           //int write_size5 = LBTClient.write("h\r\n", strlen( "h\r\n" )); //stop streaming data
           //ard_log("client first spec write_size [%d]", write_size5);       
         }
           delay(100);        
 
        //write_size = LBTClient.write((uint8_t*)"LinkIt BT Client", 16);
        //ard_log("write_size [%d]", write_size);
    }
    ard_log("loop client\n");
    delay(1000);
 
}
