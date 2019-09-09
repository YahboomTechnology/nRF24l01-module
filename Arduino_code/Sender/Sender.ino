/*
nRF24L01 Arduino Sender
nRF24L01：
nRF24L01   Arduino UNO
VCC <-> 3.3V
GND <-> GND
CE  <-> D9
CSN <-> D10
MOSI<-> D11
MISO<-> D12
SCK <-> D13
IRQ <-> no connect
*/

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

int VRX = A0; //The X-axis rocker is connected to input of analog A0 port
int VRY = A1; //The Y-axis rocker is connected to input of analog A1 port
int SW = A2;  //Rocker button is connected to A2 port

int x = 0, y = 0, z; 

void setup()
{
  Serial.begin(9600);

  Mirf.cePin = 9;          //Set the CE pin to D9
  Mirf.csnPin = 10;        //Set the CSN pin to D10
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();  //Initialize nRF24L01

  //Set the receive identifier"server"
  Mirf.setRADDR((byte *)"server");
  Mirf.payload = 3 * sizeof(unsigned int); 
  Mirf.channel = 3;
  Mirf.config();
  //The identifier here is written to "Sender.ino"
  Serial.println("I'm Sender...");
}
unsigned int adata = 0;
void loop()
{
  x = analogRead(VRX); //Read the analog voltage of A0 and assign it to x
  y = analogRead(VRY); //Read the analog voltage of A1 and assign it to Y
  z = analogRead(SW); //Read the analog voltage of A2 and assign it to z
   //Since nRF24L01 can only send Mirf.payload data in byte single byte array,
   //So all the data that needs to be transferred must be split into bytes.
   //Define the byte array below, store the pending data, because Mirf.payload = sizeof (unsigned int);
   //Actually the following is equal to byte data[6];
  byte data[Mirf.payload];
   //Split eight bits into four bits：
  data[0] = x & 0xFF;                
  data[1] = x >> 8;               
  data[2] = y & 0xFF;                
  data[3] = y >> 8;                
  data[4] = z & 0xFF;                
  data[5] = z >> 8;               
  //Serial.println(x);
  //Serial.println(y);
  //Serial.println(z);
  Mirf.setTADDR((byte *)"client");
  Mirf.send(data);
  //While the loop waits for the send to complete before proceeding to the next step
  while (Mirf.isSending()) {}
  delay(20);
}

