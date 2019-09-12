# ESP32_ArtNet_Neopixel_Driver
A dynamic, generic &amp;  N-pixel ArtNet via WIFI to Neopixel (WS2812, SK6812) driver
  - to be used with Resolume Arena 6 (+)
  - Any ESP32 Module
  - Uses the ESP32 SPI ports DATA pin to push out pixel data. Pin must be Level shifted with a SN74HCT245N of similar
  - Can drive Any number of Neopixels (WS2812, SK6812) based on the number of universes you configure

ESP32 Config Options
  - Set up your WIFI AP/ROUTER to N mode. You "CAN" use ABG or ABGN
  - Recomemnd using a dedicated WIFI Access Point not connected to any router/switch
  - Recomend conecting your PC to the PA via a LAN Cable to minimize packat loss
  - ssid = "WIFI_SSID_NETWORK_NAME";
  - password = "WIFI_PASSWORD";
  - numberOfDMXUniverses : can bet set from 1 to 255 stating the total number of universees you are pushing to this node
  - universeRange[2] : Array Index 0 is the Starting Universe ID, Array Index 1 is the Ending Universe ID. These are inclusive. EG if you are using univesres 0 to 2 inclusive. you would enter universeRange[2] = {0,2} and set numberOfDMXUniverses to 3
  - renderOnlyIfAllFramesArrive : set to 1 will only render data to the pixels when ALL Unverses have been received. Set to 0 and it will render each DMX Universe as they are received
  
Resolume Config:
 - Each universe needs to have a UNIQUE UNIVERSE ID
 - Set Output Type" to "IP Address" and use address 239.0.0.57
 - Set frame rate to under 30FPS
