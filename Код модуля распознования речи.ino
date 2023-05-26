#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <SPI.h>

VR myVR(2,3);

uint8_t records[7];
uint8_t buf[64];

#define Light (0)
#define Lamp (1)
#define Power (2)

int a;
int b;
int c;
int conn;


void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup()
{
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module");


    
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)Light) >= 0){
    Serial.println("Light Record loaded");
  }
  
  if(myVR.load((uint8_t)Lamp) >= 0){
    Serial.println("Lamp Record loaded");
  }
  if(myVR.load((uint8_t)Power) >= 0){
    Serial.println("Power Record loaded");
  }
}

void loop()
{
  
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    if(buf[1] == Light && a == 0)
      {
        a = 1;
        conn = 10;
      }
      
    else if(buf[1] == Light && a == 1)
      {
        a = 0;
        conn = 11;
      }
      
    else if(buf[1] == Lamp && b == 0)
      {
        b = 1;
        conn = 20;
      }
      
    else if(buf[1] == Lamp && b == 1)
      {
        b = 0;
        conn = 21;
      }

    else if(buf[1] == Power && c == 0)
      {
        c = 1;
        conn = 30;
      }
      
    else if(buf[1] == Power && c == 1)
      {
        c = 0;
        conn = 31;
      }
    Serial.println(conn);
    Serial.write(conn);
    printVR(buf);
  }
}
