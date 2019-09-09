/*
nRF24L01 Arduino Receiver
nRF24L01：
nRF24L01   Arduino UNO
VCC <-> 3.3V
GND <-> GND
CE  <-> D9
CSN <-> D10
MOSI<-> D11
MISO<-> D12
SCK <-> D13
IRQ <-> No connect
*/

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

unsigned int x = 0, y = 0, z = 0;

int red = 6; //Red light connect to digital port 11
int green = 5; //Green light connect to digital port 10
int blue = 3; //Blue light connect to digital port 9
int val_red = 0, val_green = 0, val_blue = 0;

void setup()
{
  Serial.begin(9600);
  
  Mirf.cePin = 9;     //Set the CE pin to D9
  Mirf.csnPin = 10;   //Set the CSN pin to D9
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();  //Initialize nRF24L01
  
  //Set the receive identifier"client"
  Mirf.setRADDR((byte *)"client");
  //sizeof(unsigned int)
  Mirf.payload = 3 * sizeof(unsigned int);
  //Send channel, can fill 0~128, send and receive must consistent。
  Mirf.channel = 3;
  Mirf.config();
  //This is used to identify the "Receiver.ino" program.
  Serial.println("I'm Receiver...");
}

void loop()
{
  //Define a temporary array of size for mirf.payload.
  byte data[Mirf.payload];
  if (Mirf.dataReady())   
  {
    Mirf.getData(data);    
    x = (unsigned int)((data[1] << 8) | data[0]);
    y = (unsigned int)((data[3] << 8) | data[2]);
    z = (unsigned int)((data[5] << 8) | data[4]);
  }
  if (z == 0) //When the joystick is pressed down, 
              //the full light is bright,white
  {
    val_red = 255;
    val_green = 255;
    val_blue = 255;
  }
  /***********************************************************************************
  The joystick is shaken and the RGB light changes its color. The value of the joystick varies by X->1023, Y->1023, X->0, Y->0 for a total of four segments.
   Each segment 1023 is 1023*4=4092, and is equally distributed to three primary colors, that is, the length of each segment is 4092/3=1364.
   Taking the red brightest point as the origin (0,0) as an example, the distance from the green brightest point (or the blue brightest point) is 1364.
   The red lightest point has a pwm value of up to 255. The map rocker value has a length of 1364, and the other brightest point has a pwm value of at least 0.
   Therefore, it is only necessary to calculate the output value of the lamp PWM with the value of the rocker value of x=0, y=0, x=1023, y=1023 in the lamp of various colors*255/1364.
  **********************************************************************************/
  else
  {
    if (y < 5)
    {
      val_red = (1364 - x) * 255 / 1364; //On the X-axis, the length of a segment is only 1023, add a segment 1364-1023=341
      //For red, the value from the brightest green is 0, increasing to the brightest red
      //The following length is calculated the same way
      val_green = x * 255 / 1364;
      val_blue = 0;
    }
    else if (x > 1020)
    {
      if (y < 341)
      {
        val_red = (341 - y) * 255 / 1364;
        val_green = (y + 1023) * 255 / 1364;
        val_blue = 0;
      }
      else
      {
        val_red = 0;
        val_green = (682 + (1023 - y)) * 255 / 1364;
        val_blue = (y - 341) * 255 / 1364;
      }
    }
    else if (y > 1020)
    {
      if (x > 341)
      {
        val_red = 0;
        val_green = (x - 341) * 255 / 1364;
        val_blue = (682 + ( 1023 - x )) * 255 / 1364;
      }
      else
      {
        val_red = (341 - x) * 255 / 1364;
        val_green = 0;
        val_blue = (1023 + x) * 255 / 1364;
      }
    }
    else if (x < 5)
    {
      val_red = (682 + ( 1023 - y )) * 255 / 1364;
      val_green = 0;
      val_blue = y * 255 / 1364;
    }
    else
    {
      val_red = 0;
      val_green = 0;
      val_blue = 0;
    }
  }
  analogWrite(red, val_red);
  analogWrite(green, val_green);
  analogWrite(blue, val_blue);
  //Serial.print
  /*    Serial.print("A0=");
      Serial.println(adata_x);
      Serial.print("A1=");
      Serial.println(adata_y);
      Serial.print("A2=");
      Serial.println(adata_z);*/
}
