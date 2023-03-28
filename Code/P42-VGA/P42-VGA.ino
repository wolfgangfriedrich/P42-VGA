// Demo Sketch for P42 Display Shield
// written by
// 2018/Feb/11
// Wolfgang Friedrich
// https://hackaday.io/project/21097-arduino-video-display-shield
// https://github.com/wolfgangfriedrich/P42Display


// supported resolutions:
//  NTSC 320x200x8bit 
//  PAL  300x240x8bit
// work in progress VGA PAL 640x480x2bit

#include <Arduino.h>
#include <SPI.h>

// *** Set Video mode and Uno/Mega platform in this header file ***
#include "VS23S040D-VGA.h"

P42Display P42Display; 

void setup() {
	unsigned int P42_Display_ID = 0;

	Serial.begin(115200);
	Serial.println("");
	Serial.println(F("P42 VGA Shield Test"));

	P42_Display_ID = P42Display.Config( CH0 );

}

//void SPImemdump (unsigned long address, unsigned int bytes) {
//
//	unsigned long i = 0;
//	byte result =0;
//	
//	digitalWrite(MemSelectPin,LOW);
//	SPI.transfer( 0x03 );					// read data
//	SPI.transfer( (address >>16) &0xFF );	// start address
//	SPI.transfer( (address >> 8) &0xFF );
//	SPI.transfer( (address     ) &0xFF );
//	for (i = 0; i < bytes; i++) {
//		if ((i % 8) == 0) {
//			if (i != 0)
//				Serial.println (";");
//			Serial.print(" 0x");
//			Serial.print(address + i,HEX);
//			Serial.print(": ");
//		}
//		result = SPI.transfer( 0x00 );
//		Serial.print(" 0x");
//		if (result <0x10)
//			Serial.print ("0");
//		Serial.print(result,HEX);
//	}
//    digitalWrite(MemSelectPin,HIGH); 
//	Serial.println("<");
//  
//
//}


void loop() {

	word adc_value = 0;
	byte incomingByte = 0;   // for incoming serial data
	byte GPIOControlReg = 0;
	u_int32 i,j,x,y; // Used for counters
	u_int32 address;

// SPImemdump ( 0, 0x100 );



	Serial.println(F("Image address map "));
	
	address = LINE_ADDR_8(0) + 0;
	Serial.print(F("Address pixel (0,0): 0x"));
	Serial.println(address, HEX );
	address = LINE_ADDR_8(0) + 1;
	Serial.print(F("Address pixel (1,0): 0x"));
	Serial.println(address, HEX );
	address = LINE_ADDR_8(0) + 2;
	Serial.print(F("Address pixel (2,0): 0x"));
	Serial.println(address, HEX );
	address = LINE_ADDR_8(1) + 0;
	Serial.print(F("Address pixel (0,1): 0x"));
	Serial.println(address, HEX );
	address = LINE_ADDR_8(VISIBLE_PIXELS-1) + VISIBLE_LINES-1;
	Serial.print(F("Address pixel ("));
	Serial.print(VISIBLE_PIXELS-1);
	Serial.print(F(","));
	Serial.print(VISIBLE_LINES-1);
	Serial.print(F("): 0x"));
	Serial.println(address, HEX );

	Serial.println(F("Draw Test bytes [press key]"));
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();
//	P42Display.SPIWriteByte (2,0x10f4, 0xff, false);
//	P42Display.SPIWriteByte (2,0x10f5, 0xaa, false);
	P42Display.SPIWriteByte (2,0x177c<<1, 0xeb, false);
//	P42Display.SPIWriteByte (2,0x118a, 0xff, false);

	for (i=0; i< 240; i++) {
		P42Display.SPIWriteByte (2,(0x177c<<1)+i, 0xeb, false);
		P42Display.SPIWriteByte (1,0x2ef8+i, 0xff, false);
	}
	
	
	Serial.println(F("Clear Screen [press key]"));
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();
	P42Display.ClearScreen ( CH1, 0x00 );
	P42Display.ClearScreen ( CH2, 0x00 );
	P42Display.ClearScreen ( CH3, 0x00 );

// test image memory
	for (i=0xf08; i<0x1e180; i++)
//	P42Display.SPIWriteByte (i, i & 0xff, false);

	Serial.println(F("Fill Screen [press key]"));
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();
//	P42Display.ClearScreen ( 0xff );

	Serial.println(F("Draw Test Image [press key]"));
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();

// Draw test image
	Serial.println("Draw test image");
//	Serial.println("Draw test image [press key] ");
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();

	#define XSIZEREC VISIBLE_PIXELS/2
	#define YSIZEREC VISIBLE_LINES/2
	for (i=0; i<2; i++) for (j=0; j<2; j++) {
//		P42Display.FilledRectangle((i*XSIZEREC),              (j*YSIZEREC),              (i*XSIZEREC)+(XSIZEREC-1), (j*YSIZEREC)+(YSIZEREC-1), (j*16)+i);	// Draw colored rectangles
//		P42Display.FilledRectangle((i*XSIZEREC),              (j*YSIZEREC)+(YSIZEREC-1), (i*XSIZEREC)+(XSIZEREC-1), (j*YSIZEREC)+(YSIZEREC-1), 0);			// Draw black horizontal line
//		P42Display.FilledRectangle((i*XSIZEREC)+(XSIZEREC-1), (j*YSIZEREC),              (i*XSIZEREC)+(XSIZEREC-1), (j*YSIZEREC)+(YSIZEREC-1), 0);			// Draw black vertical line
	}

	Serial.println(F("Clear Screen [press key]"));
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();

	P42Display.ClearScreen ( CH0, 0x00 );

//	Serial.println(F("4 RGB Pixel [press key]"));
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();
//
//	x = 0;
//	y = 0;
//	for (i = 0; i < YPIXELS; i++)
//		for (j = x; j < XPIXELS/8; j++)
//			P42Display.SetRGBPixel (  j,   i, 0x00ff0000);
//	x = XPIXELS/8;
//	y = 0;
//	for (i = 0; i < YPIXELS; i++)
//		for (j = x; j < 2*XPIXELS/8; j++)
//			P42Display.SetRGBPixel (  j,   i, 0x0000ff00);
//	x = 2*XPIXELS/8;
//	y = 0;
//	for (i = 0; i < YPIXELS; i++)
//		for (j = x; j < 3*XPIXELS/8; j++)
//			P42Display.SetRGBPixel (  j,   i, 0x000000ff);
//	x = 3*XPIXELS/8;
//	y = 0;
//	for (i = 0; i < YPIXELS; i++)
//		for (j = x; j < 4*XPIXELS/8; j++)
//			P42Display.SetRGBPixel (  j,   i, 0x00ffffff);
//	
//	Serial.println("4 YUV Pixel [press key]");
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();
//
////	P42Display.ClearScreen ( 0x00 );
//	x = 4*XPIXELS/8;
//	y = 0;
//	for (i = y; i < YPIXELS; i++)
//		for (j = x; j < 5*XPIXELS/8; j++)
//			P42Display.SetYUVPixel (  j,   i, 0x22);
//	x = 5*XPIXELS/8;
//	y = 0;
//	for (i = y; i < YPIXELS; i++)
//		for (j = x; j < 6*XPIXELS/8; j++)
//			P42Display.SetYUVPixel (  j,   i, 0x92);
//	x = 6*XPIXELS/8;
//	y = 0;
//	for (i = y; i < YPIXELS; i++)
//		for (j = x; j < 7*XPIXELS/8; j++)
//			P42Display.SetYUVPixel (  j,   i, 0x52);
//	x = 7*XPIXELS/8;
//	y = 0;
//	for (i = y; i < YPIXELS; i++)
//		for (j = x; j < 8*XPIXELS/8; j++)
//			P42Display.SetYUVPixel (  j,   i, 0xbf);
//	
//	Serial.println(F("Character [press key]"));
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();
//
//	P42Display.PrintChar ('4', 0, 40, 0x15);
//	P42Display.PrintChar ('2', 8, 40, 0x15);
//	
//	Serial.println(F("All characters [press key]"));
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();
//
//	P42Display.ClearScreen ( 0x02 );
//	P42Display.PrintString ("!\"#$%&\'()*+,-./0123456789:;<=>?@", 0, 0, 0x05);
//	P42Display.PrintString ("ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`", 0, 16, 0x05);
//	P42Display.PrintString ("abcdefghijklmnopqrstuvwxyz{|}~", 0, 32, 0x05);
//
//	Serial.println("String [press key]");
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();
//
//	P42Display.ClearScreen ( 0x5c );
//	P42Display.PrintString ("**** ARDUINO P42 DISPLAY SHIELD ****", 8, 0, 0x55);
//	P42Display.PrintString ("2M FLASH SYSTEM 261368 BYTES FREE.", 16, 16, 0x54);
//	P42Display.PrintString ("READY.", 0, 32, 0x53);
//	while (Serial.available() == 0) {
//		P42Display.PrintChar ('_', 0, 40, 0x53);
//		delay (500);
//		P42Display.PrintChar ('_', 0, 40, 0x5c);
//		delay (500);
//	};
//
//	Serial.println("2 colour select [press key]");
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();
//	
//	P42Display.ClearScreen ( 0x02 );
//	for (y=0; y < YPIXELS-8; y = y+8) {
//		for (x=0; x < XPIXELS-16; x = x+16 ) {
//			P42Display.FilledRectangle ( x, y, x+15, y+7, y/8*16 + x/16  + 1 );
//			P42Display.FilledRectangle ( x+ 2, y+2, x+ 6, y+5, y/8*16 + x/16  + 1 + 2 );
//			P42Display.FilledRectangle ( x+10, y+2, x+13, y+5, y/8*16 + x/16  + 1 - 2 );
////			P42Display.PrintChar ('X', x  , y, y/8*16 + x/16  + 1 + 2 );
//	//		P42Display.PrintChar ('X', x+8, y, y/8*16 + x/16  + 1 - 2 );
//		}
//	}

	Serial.println(F("End of test! [Restart press key]"));
	delay(1);
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();
}


