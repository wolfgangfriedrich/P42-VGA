# P42-VGA
Initial publish of Rev2 data for Tindie product page


Description
A VGA shield/wing based on the VLSI VS23S040 chip. The board has 2 sets of connectors, one for a standard Arduino Uno shield and one to connect to a Adafruit feather board as a Wing. The Wing connectors also makes is compatible to breadboards. Also 4 channel composite NTSC/PAL video output is possible with the correct crystal. A SOIC-8 standard pinout SPI flash memory is on board for graphics and character sets, as well as a I2C EEPROM also in SOIC-8 package. A level translator chip takes care of the correct voltage levels, so a 5V or 3.3V Arduino could be used (Uno or Zero). The SPI signals are routed through a pin header in line with the ICSP connector, so the shield can be used on an UNO, Mega, Due or Zero with the hardware SPI interface. It is the next generation of my Arduino Video Display Shield.https://hackaday.io/project/21097-ntscpal-video-display-shie
Details
This section can be used to explain everything about your project.

The VLSI VS23S040 is the successor of the trusted VS23S010 composite video chip with framebuffer. This Shield/Wing uses the VS23S040 to generate VGA signals or composite video.

Currently it is running as 4x composite video output with resolution NTSC 320x200 in 256 colours and PAL 300x240 in 256 colours.

I plan to get it running in the future as VGA with 640x480 resolution with  3bit per RGB to generate 512 colours.   
