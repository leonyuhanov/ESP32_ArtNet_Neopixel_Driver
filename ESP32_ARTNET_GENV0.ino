/*
     Tested on the NODEMCU-32S
     Noopixel Output Pin is  [SPI MOSI]-(PIN 23)-GPIO23 
*/
#include "SPI.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#include "NeoViaSPI.h"
#include "artNetPacket.h"

//networking
const char * ssid = "WIFISSID";
const char * password = "WIFIKEY";
unsigned int artNetPort = 6454;
const short int maxPacketBufferSize = 530;
char packetBuffer[maxPacketBufferSize];
AsyncUDP udp;
short int packetSize=0;
artNetPacket dmxData;

//DMX Config - This example is set up for 3 Universes with ids 0,1 & 2
const byte numberOfDMXUniverses = 3;
const unsigned short int universeRange[2] = {0,2};  //  [Starting Universe ID, Ending Universe ID] (inclusive)
//Set to 1 to only render to the LEDs when ALL DMX frames havea arrived
byte renderOnlyIfAllFramesArrive = 0;

//modify for dynmic
byte frameChecks[numberOfDMXUniverses][2];
unsigned short int dmxIndex=0, frameCntBreak=0, frameTicks=0, frameTCnt=0, innerFTCnt=0, startPixel=0, pxIndex=0;
const byte numberOfPixelsPerUniverse = 170;
const unsigned short int numLeds = numberOfDMXUniverses*numberOfPixelsPerUniverse;

NeoViaSPI leds = NeoViaSPI(numLeds);
byte tempColour[3] = {0,0,0};

//unsigned long frameTimers[2];

void setup()
{
  Serial.begin(115200);
  Serial.print("\r\n\r\n");

  //Eable WIFI
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ArtNetNode");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
        delay(100);
        Serial.print(".");
  }
  Serial.print("\r\nONLINE\t");
  Serial.print(WiFi.localIP());

  //Set up UDP
  
  udp.listen(artNetPort);
  udp.onPacket(pollDMX);
   
  //Init SPI for physical Pixel Driver
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(3333333);

  //set up DMX Frame Check-in system
  for(frameTCnt=0; frameTCnt<numberOfDMXUniverses; frameTCnt++)
  {
    //Set This frames ID
    frameChecks[frameTCnt][0] = frameTCnt;
    //Set this frame to be clear = 0
    frameChecks[frameTCnt][1] = 0;
  }

  //clear all pixels
  renderLEDs();
}

void renderLEDs()
{
  leds.encode();
  SPI.writeBytes(leds.neoBits, leds._NeoBitsframeLength);
  delayMicroseconds(80);
}

//ARTNET STUFF
void pollDMX(AsyncUDPPacket &packet)
{
     packetSize = packet.length();
     //Serial.println(packetSize);
     if(packetSize==maxPacketBufferSize)
     {
        memcpy(packetBuffer, packet.data(), maxPacketBufferSize);
        //packet.read(packetBuffer, maxPacketBufferSize);
        //Serial.printf("\r\n\tUDP Packet Received for Univirse\t[%d.%d]", packetBuffer[14], packetBuffer[15]);
        //packetBuffer[14] is the UNIVERSE byte check that it is within the range cinfigured above
        if(packetBuffer[14]>=universeRange[0] && packetBuffer[14]<=universeRange[1])
        {
          dmxData.parseArtNetPacket(packetBuffer);
        }
     }
     //udp.flush();
}

void artNetToSPI(byte panelID)
{
  startPixel = panelID * numberOfPixelsPerUniverse;
  dmxIndex = 0;

  //Serial.printf("\r\n\t\tInside artnet to spi. Starting at pixel\t%d", startPixel);

  for(pxIndex=startPixel; pxIndex<startPixel+numberOfPixelsPerUniverse; pxIndex++)
  {
    tempColour[0] = dmxData.data[dmxIndex];
    tempColour[1] = dmxData.data[dmxIndex+1];
    tempColour[2] = dmxData.data[dmxIndex+2];
    leds.setPixel(pxIndex, tempColour);
    dmxIndex+=3;
  }
  //Serial.printf("\r\n\t\t\tpxIndex\t%d\tdmxIndex\t%d", pxIndex, dmxIndex);

  //Check if RenderAllFrames mode is enabled
  if(renderOnlyIfAllFramesArrive)
  {
    //Check in this frames data to see if the entire canvas should be rendered
    if(checkFrame(panelID))
    {
      //frameTimers[1] = millis();
      //Serial.printf("\r\n\t\t\tAll Frames in rendering now\t%d\tMilliseconds", frameTimers[1]-frameTimers[0]);
      renderLEDs();
      //frameTimers[0] = millis();
    }
  }
  else
  {
    //frameTimers[1] = millis();
    //Serial.printf("\r\n\t\t\tFrame\t%d\t in rendering now\t%d\tMilliseconds", panelID, frameTimers[1]-frameTimers[0]);
    renderLEDs();
    //frameTimers[0] = millis();
  }
}

byte checkFrame(byte dmxFrameID)
{
  frameTicks = 0;
  
  //If all frames are OLD begin full check
  if(frameCntBreak==0)
  {
    frameCntBreak=1;
  }
  
  //Go through all frames and see which ones data has arrive
  for(frameTCnt=0; frameTCnt<numberOfDMXUniverses; frameTCnt++)
  {
    if( frameChecks[frameTCnt][0] == dmxFrameID && frameChecks[frameTCnt][1]==0)
    {
      //Current frame was old and is now NEW
      frameChecks[frameTCnt][1] = 1;
      break;
    }
    else if(frameChecks[frameTCnt][0] == dmxFrameID && frameChecks[frameTCnt][1]==1)
    {
      //Current frame was already writen to in this round
      //Frames are out of sync reject render this time round
      frameCntBreak=0;
      //clear all frame checks
      for(innerFTCnt=0; innerFTCnt<numberOfDMXUniverses; innerFTCnt++)
      {
        frameChecks[innerFTCnt][1]=0;
      }
      break;
    }
  }
  //if still checking
  if(frameCntBreak)
  {
    for(frameTCnt=0; frameTCnt<numberOfDMXUniverses; frameTCnt++)
    {
      if(frameChecks[frameTCnt][1]==1)
      {
        frameTicks++;
      }
    }
    //if all frames are checked
    if(frameTicks==numberOfDMXUniverses)
    {
      frameCntBreak=0;
      //Frames IN sync
      //clear checks
      for(frameTCnt=0; frameTCnt<numberOfDMXUniverses; frameTCnt++)
      {
        frameChecks[frameTCnt][1]=0;
      }
      //return render
      return 1;
    }
  }
  //return DO NOT render
  return 0;
}

void loop()
{    
    //ArtNet Mode
    //pollDMX();
    if(dmxData.hasChanged)
    {
      //Serial.printf("\r\n\t\tProcessing DMX DATA For U\t[%d.%d]", dmxData.universe[0],dmxData.universe[1]);
      dmxData.hasChanged = 0;
      artNetToSPI(dmxData.universe[0]);
    }
    yield(); 
}
