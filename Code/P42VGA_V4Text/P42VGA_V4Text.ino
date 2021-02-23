// Demo Sketch for P42 VGA Shield
//  Using Channel0 Composite Video Output
//
// The idea is to have messages from 2 serial ports being displayed and scrolled up.
// I would call it semi-duplex, as ideally one side sends a command/line and the the other side.
// If both sides send characters at the same time, they get mixed with each other in the same line.
// Different colours for the 2 sides are used, so it is visible if that happens. 
//
// HW Setup:
// Arduino Mega with P42 Video4 VGA Shield
// Standard serial port setup as SERIAL 115200 8N1
// 2nd serial port setup as SERIAL1 38400 8N1 (this is also a hardware serial port on the Mega on pins 18:TX 19:RX
// The baudrate is set to 38400 to be operational with the Arduino SoftwareSerial library as well.
// For the 2nd serial port a TTl-UART adapter is required with connections to RX, TX and GND. 
// 
// SW Setup:
// !!! The Arduino Serial Monitor is not suitable for this setup.
// A proper terminal software is required, I use two instances of TeraTerm, 
// configured with the correct serial settings, local echo and transmit CR+LF.
//
// Description:
// The global variables ScreenX and ScreenY as well as the function printlineXY() are the heart of this demo.
//
// ScreenX, ScreenY point to the coordinate of the screen where the next character will be printed.
// Function: byte printlineXY ( byte channel, char* Text, byte colour, bool ln )
//   Paramters:
//   byte channel: is the video channel used for display; can be one of CH0, CH1, CH2, CH3.
//   char* Text: pointer to a string, null terminated. To print a single character, the character + a NULL byte need to be content of the string.
//               Maximum string length is the remainder of the current line + a full additional line as only one linebreak is currently implemented.
//   byte colour: a 8-bit colour value picked from the colour table in the user guide.
//   bool ln: of TRUE a CR+LF is performed to start a new line after the string got printed.
//    byte return value: currently always 0.
//
//  At the bottom of the screen, if the last line is full or LF = TRUE, a line scrolling is implemented to make room for the next line of text. 
//  Implemented control codes:
//  0x08 : Backspace, the last character gets deleted to free the area for the next character to be printed, so technically it is a 'delete char'
//  0x0A : LF Linefeed, cursor go to next line, but same horizontal position 
//  0x0D : CR Carriage return, cursor goes to the beginning of the current line
//
// More information with pictures and wiring diagram will appear on:
// https://hackaday.io/project/170499-vga-shield-wing
// SW repository
// https://github.com/wolfgangfriedrich/P42-VGA
//
// written by
// Wolfgang Friedrich
// Last change: 2021/Feb/21
// 



#include <Arduino.h>
#include <SPI.h>

// *** Set Video mode in this header file ***
#include "VS23S040D.h"


//#include <SoftwareSerial.h>

#ifdef ArduinoShield
//SoftwareSerial Serial1(22,24); // RX, TX
#endif
#ifdef FeatherWing
SoftwareSerial Serial1(12, 13); // RX, TX
#endif


P42Display P42Display; 

#define Background 0x5c

// global variables
u_int16 ScreenX = 0;
u_int16 ScreenY = 0;

byte printlineXY ( byte channel, char* Text, byte colour, bool ln ) {

const u_int32 MEMBYTES_LINE 		= (PICLINE_BYTE_ADDRESS(1) + 0) - (PICLINE_BYTE_ADDRESS(0) + 0);

	// enable the channel(s) that are handed in.
	P42Display.SPIWriteRegister (WriteMultiICAccessControl, 0x0f ^ (1 << channel), false);

	// Control Characters
	if ( Text[0] == 0x0a)  {
		ScreenY = ScreenY + 8;
	}
	else if ( Text[0] == 0x0d)  {
		ScreenX = 0;
	}
	else if (( Text[0] == 0x08) and (ScreenX > 8))  {
		ScreenX = ScreenX - 8;
		P42Display.FilledRectangle (channel, ScreenX, ScreenY, ScreenX+7, ScreenY+7, Background);
	}
	else if ( strlen(Text) > (XPIXELS-1-ScreenX)/8) {
	// Print text to the end of line
		Text[strlen(Text) - (XPIXELS-1-ScreenX)/8] = 0;
		P42Display.PrintString ( channel, Text, ScreenX, ScreenY, colour);
		ScreenX = 0;
		ScreenY = ScreenY + 8;
		// deal with the rest of the string!!!
	}
	else {
	// Print text
		P42Display.PrintString ( channel, Text, ScreenX, ScreenY, colour);
		ScreenX = ScreenX + 8*strlen(Text);
		if ((ln) or (ScreenX > XPIXELS-1)) {
			ScreenX = 0;
			ScreenY = ScreenY + 8;
		}
	}

	if (ScreenY > YPIXELS-8 ) {
		// scroll
		// wait for line counter to be outside of visible area
		while ( P42Display.SPIReadRegister16 ( ReadCurrentLinePLL, false) & 0x01ff < ENDLINE ) {}
		
		// move screen 1st 255 pixels
		P42Display.SPIWriteRegister40 (WriteBlockMoveControl1, 
			((u_int32)(PICLINE_BYTE_ADDRESS(8) >>1 & 0xFFFF)), 
			((PICLINE_BYTE_ADDRESS(0)) >>1 & 0xFFFF), 
			BMVC_PYF | ((u_int32)(PICLINE_BYTE_ADDRESS(8)) & 0x00001) <<2 | ((PICLINE_BYTE_ADDRESS(0)) & 0x00001) <<1 , false);
		P42Display.SPIWriteRegister32 (WriteBlockMoveControl2, (u_int32)((u_int32)(MEMBYTES_LINE-0xff)<<16) |(u_int16) (0xff<<8) | (YPIXELS-1)-8, false);
		P42Display.SPIWriteRegister (StartBlockMove, 0x00 , false);
		
		// test wait
		delay (40);
		
		// move screen leftover pixels
		P42Display.SPIWriteRegister40 (WriteBlockMoveControl1, 
			((u_int32)((PICLINE_BYTE_ADDRESS(8)+256) >>1 & 0xFFFF)), 
			((PICLINE_BYTE_ADDRESS(0)+256) >>1 & 0xFFFF), 
			BMVC_PYF | ((u_int32)(PICLINE_BYTE_ADDRESS(8)+256) & 0x00001) <<2 | ((PICLINE_BYTE_ADDRESS(0)+256) & 0x00001) <<1 , false);
		P42Display.SPIWriteRegister32 (WriteBlockMoveControl2, (u_int32)((u_int32)(MEMBYTES_LINE-(XPIXELS-0xff))<<16) |(u_int16) ((XPIXELS-0xff)<<8) | (YPIXELS-1)-8, false);
		P42Display.SPIWriteRegister (StartBlockMove, 0x00 , false);
		
		// test wait
		delay (40);

		// clear last text line
		P42Display.FilledRectangle (channel, 0, YPIXELS-8, XPIXELS-1, YPIXELS-1, Background);
		
		ScreenY = YPIXELS-8;

	}
	
	return 0;
		
}



void setup() {
	u_int16 P42_Display_ID = 0;

	while (!Serial) ;

	Serial.begin(115200);
	Serial.println("");
	Serial.println(F("P42 VGA Shield Test - Composite Output"));

	// Config pins
	pinMode(nWPPin, OUTPUT);
	digitalWrite(nWPPin, HIGH);
	pinMode(nHOLDPin, OUTPUT);
	digitalWrite(nHOLDPin, HIGH);

	// Disable pins that are used for SPI on Uno.
	pinMode(11, INPUT);
	pinMode(12, INPUT);
	pinMode(13, INPUT);

	// Config SPI interface
	pinMode(slaveSelectPin, OUTPUT);
	digitalWrite(slaveSelectPin, HIGH);
	pinMode(MemSelectPin, OUTPUT);
	digitalWrite(MemSelectPin, HIGH);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST) ; 
	
	P42_Display_ID = P42Display.Config( CH0 );
	
	// set the data rate for the SoftwareSerial port
	Serial1.begin(38400);
//	Serial1.println("Hello, world?");

	ScreenX = 0;
	ScreenY = 0;

}


void loop() {

	word adc_value = 0;
	byte incomingByte = 0;   // for incoming serial data
	byte GPIOControlReg = 0;
	u_int32 i,j,x,y; // Used for counters
	u_int32 address;
	byte value;
	
	u_int32 Ser_Pos = 0;
	u_int32 Ser1_Pos = 0;
	
	char Ser_RX [33] = "Ser_RX";
	char Ser1_RX[33] ="Hello World";
	
	P42Display.ClearScreen ( CH0, 0x00 );

	P42Display.ClearScreen ( CH0, Background );
	printlineXY ( CH0, " **** ARDUINO P42 VIDEO4 SHIELD ****", 0x55, true);
	printlineXY ( CH0, "16M FLASH SYSTEM A LOT OF BYTES FREE.", 0x54, true);
	printlineXY ( CH0, "CH0 READY.", 0x53, true);

	Serial.println(F("UART ping pong") );
	
	// testing
	strcpy ( Ser_RX, "Blub");
	printlineXY (CH0, Ser_RX, 0x53, true);
	printlineXY (CH0, Ser1_RX, 0x58, true);
	
	// endless loop to display serial single characters from 2 serial ports
	while (1){
		if (Serial.available()) {
			Ser_RX[0] = Serial.read();
			Ser_RX[1] = 0;
			Serial1.print ( Ser_RX[0] );
			printlineXY (CH0, &Ser_RX[0], 0x53, false);
		}
		if (Serial1.available()) {
			Ser1_RX[0] = Serial1.read();
			Ser1_RX[1] = 0;
			Serial.print ( Ser1_RX[0] );
			printlineXY (CH0, &Ser1_RX[0], 0x58, false);
		}
	}

} // The End.
