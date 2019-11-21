# ESP32_ArtNet_Neopixel_Driver

Note there are seperate projects:
  - ASYNCUDP uses the ESP32 Async UDP driver, this is faster but I have had issues with stability so treat this as a prototype
  - STDUDP uses the standard ESP32 UDP class & is solid

A dynamic, generic &amp;  N-pixel ArtNet via WIFI to Neopixel (WS2812, SK6812) driver
  - to be used with Resolume Arena 6 (+)
  - Any ESP32 Module
  - Uses the ESP32 SPI ports DATA pin to push out pixel data. Pin must be Level shifted with a SN74HCT245N of similar
  - Can drive Any number of Neopixels (WS2812, SK6812) based on the number of universes you configure

ESP32 Config Options
  - Set up your WIFI AP/ROUTER to N mode. You "CAN" use ABG or ABGN
  - Recomend using a dedicated WIFI Access Point not connected to any router/switch
  - Recomend conecting your PC to the Access Point via a LAN Cable to minimize packat loss
  - ssid = "WIFI_SSID_NETWORK_NAME";
  - password = "WIFI_PASSWORD";
  - IF YOU ARE MAKING MORE THAN 1 NODE On Line 45 set a different hostname for each one WiFi.setHostname("HOST_NAME_OF_YOUR_CHOOSING");
  - numberOfDMXUniverses : can bet set from 1 to 255 stating the total number of universees you are pushing to this node
  - universeRange[2] : Array Index 0 is the Starting Universe ID, Array Index 1 is the Ending Universe ID. These are inclusive. EG if you are using univesres 0 to 2 inclusive. you would enter universeRange[2] = {0,2} and set numberOfDMXUniverses to 3
  - renderOnlyIfAllFramesArrive : set to 1 will only render data to the pixels when ALL Unverses have been received. Set to 0 and it will render each DMX Universe as they are received
  
Resolume Config:
 - Each universe needs to have a UNIQUE UNIVERSE ID
 - If you are using MANY Nodes -> Set Output Type" to "IP Address" and use the devices IP Address
 - If you are using 1 Node -> Set Output Type" to "BROADCAST"
 - Fixtures need to have pixels orderd in RGB,RGB,RGB etc...
