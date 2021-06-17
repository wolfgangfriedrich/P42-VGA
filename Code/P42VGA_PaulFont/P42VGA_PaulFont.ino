// Demo Sketch for P42 VGA Shield and PAUL12x18 font
//  Using 4x Composite Video Output
// written by
// Wolfgang Friedrich
// Last change: 2021/Jun/16
// https://hackaday.io/project/170499-vga-shield-wing
// https://github.com/wolfgangfriedrich/P42-VGA


// supported resolutions:
//  NTSC 320x200x8bit 
//  PAL  300x240x8bit

#include <Arduino.h>
#include <SPI.h>
#include "Paul12x18font.h"

// *** Set Video mode in this header file ***
#include "VS23S040D.h"

P42Display P42Display; 

#define Background	0x00

//-----------------------------------------------------------------------------
// Function PrintPaulChar ()
// 
// Print a single character out of the Paul12x18font.h character set.
//  Supported ASCII codes are 32 - 122(dec) and special characters 'éöíóñ'.
//
// Parameters:
// byte channel: values 0-3 to print on the stated output channel
// char Letter: ASCII code of the character to print, special characters have the following codes:
//    169:	// 'é':
//    182:	// 'ö':
//    173:	// 'í':
//    179:	// 'ó':
//    177:	// 'ñ':

// word x: x coordinate of top left corner
// word y: y coordinate of top left corner
// byte colour: colour of the set pixels,picked from the clour chart in the VGA shield user guide
//----------------------------------------------------------------------------- 

void PrintPaulChar ( byte channel, char Letter, word x, word y, byte colour ) {

u_int16 bitmap[18];
byte xp,yp = 0;		// xpixel and ypixel
u_int32 char_address;
	
	switch ( Letter ) {
//		case 'é':
		case 169:	// 'é':
			char_address = ('z' - ' ' + 1) *18 + 3;
			break;
		case 182:	// 'ö':
			char_address = ('z' - ' ' + 2) *18 + 3;
			break;
		case 173:	// 'í':
			char_address = ('z' - ' ' + 3) *18 + 3;
			break;
		case 179:	// 'ó':
			char_address = ('z' - ' ' + 4) *18 + 3;
			break;
		case 177: 	//'ñ':
			char_address = ('z' - ' ' + 5) *18 + 3;
			break;
//		case ' ':
//			char_address = (0) *18 + 3;
//			break;
		default:
			char_address = (Letter - ' ') * 18 + 3;  // Startword of character +3 words at the beginning for field dimensions
			break;
	}

//		Serial.print(F("Char address: 0x"));
//		Serial.println(char_address, HEX);
//		Serial.print(F(" |"));
//		Serial.print(Letter, DEC);
//		Serial.print(F(" "));
		
//		Serial.print(F(" >"));
//		Serial.print(char_address, DEC);
//		Serial.print(F(" "));

	
	for (yp = 0; yp < font12x18ext[1]; yp++)		// repeat for all lines of character   
	{
//		Serial.print(F(" 0x"));
//		Serial.print(font12x18ext[char_address], HEX);
		
		for (xp = font12x18ext[0]; xp > 0; xp--)
		{
//			Serial.print(xp);
	// Add boundary check.
			if ((x+12-xp < XPIXELS) and (y+yp < YPIXELS) ) {
				if ((font12x18ext[char_address + yp] >> (xp-1) ) & 0x01 ) {
					P42Display.SetYUVPixel ( channel, x+12-xp, y+yp, colour);
//					Serial.print(F("."));
				}
				else {
					P42Display.SetYUVPixel ( channel, x+12-xp, y+yp, Background);
//					Serial.print(F("_"));
				}
			}
		}	
//		Serial.println();
	}
}


//-----------------------------------------------------------------------------
// Function PrintPaulString ()
// 
// Print a string of character out of the Paul12x18font.h character set.
//  Supported ASCII codes are 32 - 122(dec) and special characters 'éöíóñ'.
//
// Parameters:
// byte channel: values 0-3 to print on the stated output channel
// char* Letter: string of chars, to be terminated with 0x00 to indicate end. 
// word x: x coordinate of top left corner
// word y: y coordinate of top left corner
// byte colour: colour of the set pixels,picked from the clour chart in the VGA shield user guide
//
// Note: As the special characters are coded in a 2-byte format, the strlen()
//       function, will be off by +1 for every special character in the current string.
//----------------------------------------------------------------------------- 

void PrintPaulString (byte channel, char* Text, word x, word y, byte colour) {

char Letter;
u_int16 i,j = 0;
u_int16 x_loc = 0;

	Letter = Text[i];
	x_loc = x;
//	Serial.print (Letter);
//	Serial.println (strlen(Text), DEC);
	
	while (Text[i] != 0x00) {
//		Serial.print (F('<'));
		if (Text[i] == 195) {
//			i--;		// remove extra char for umlaute because UTF-8 encoding
		}
		else if (((Text[i] >= ' ') && (Text[i] <= 'z')) || (Text[i] = 'é') || (Text[i] = 'ö') || (Text[i] = 'í') || (Text[i] = 'ó') || (Text[i] = 'ñ')) {			// char between 0x20 and 0x7A plus 5 umlaute
//			Serial.print (x_loc, DEC);
//			Serial.print (F('<'));
			PrintPaulChar (channel, Text[i], x_loc, y, colour);
			x_loc = x_loc + font12x18ext[0];
		}
		i++;
	}
//	Serial.println();

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
#ifdef MEGA
	pinMode(11, INPUT);
	pinMode(12, INPUT);
	pinMode(13, INPUT);
#endif	

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
	P42_Display_ID = P42Display.Config( CH1 );
	P42_Display_ID = P42Display.Config( CH2 );
	P42_Display_ID = P42Display.Config( CH3 );

}


void loop() {

	word adc_value = 0;
	byte incomingByte = 0;   // for incoming serial data
	byte GPIOControlReg = 0;
	u_int32 i,j,x,y; // Used for counters
	u_int32 address;
	byte value;
	
	Serial.println(F("Image address map "));
	
	address = PICLINE_BYTE_ADDRESS(0) + 0;
	Serial.print(F("Address pixel (0,0): 0x"));
	Serial.println(address, HEX );
	address = PICLINE_BYTE_ADDRESS(0) + 1;
	Serial.print(F("Address pixel (1,0): 0x"));
	Serial.println(address, HEX );
	address = PICLINE_BYTE_ADDRESS(0) + 2;
	Serial.print(F("Address pixel (2,0): 0x"));
	Serial.println(address, HEX );
	address = PICLINE_BYTE_ADDRESS(1) + 0;
	Serial.print(F("Address pixel (0,1): 0x"));
	Serial.println(address, HEX );
	address = PICLINE_BYTE_ADDRESS(10) + 10;
	Serial.print(F("Address pixel (10,10): 0x"));
	Serial.println(address, HEX );
	address = PICLINE_BYTE_ADDRESS(YPIXELS-1) + XPIXELS-1;
	Serial.print(F("Address pixel ("));
	Serial.print(XPIXELS);
	Serial.print(F(","));
	Serial.print(YPIXELS);
	Serial.print(F("): 0x"));
	Serial.println(address, HEX );
	

	
//	Serial.println(F("Start Test and Clear Screen [press key]"));
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();

	P42Display.ClearScreen ( CH0, 0x00 );
	P42Display.ClearScreen ( CH1, 0x00 );
	P42Display.ClearScreen ( CH2, 0x00 );
	P42Display.ClearScreen ( CH3, 0x00 );

	// single character and string test print

//	Serial.println(F("Character [press key]"));
//	while (Serial.available() == 0) {};
//	incomingByte = Serial.read();

	P42Display.PrintChar ( CH0, '4', 0, 40, 0xEE);
	P42Display.PrintChar ( CH0, '2', 8, 40, 0xEE);
	
	PrintPaulChar ( CH0, 'z',  0, 50, 0xEE);
	PrintPaulChar ( CH0, 'é', 12, 50, 0xEE);
	PrintPaulChar ( CH0, 123, 24, 50, 0xEE);

	Serial.println();
	
	PrintPaulString ( CH0, "éöíóñ", 0, 70, 0x0c );
	PrintPaulString ( CH0, "éöíóñ", 290, 70, 0x0c );
	PrintPaulString ( CH0, "éöíóñ", 0, 190, 0x0c );
	PrintPaulString ( CH0, "éöíóñ", 280, 185, 0x0c );
	
	Serial.println();
	
	P42Display.PrintChar ( CH1, '1', 8, 40, 0xEE);
	P42Display.PrintChar ( CH2, '2', 8, 40, 0xEE);
	P42Display.PrintChar ( CH3, '3', 8, 40, 0xEE);
	
	
	// String output all characters
	Serial.println(F("All characters [press key]"));
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();

	P42Display.ClearScreen ( CH0,  0x00 );
	P42Display.ClearScreen ( CH1,  0x00 );
	P42Display.ClearScreen ( CH2,  0x00 );
	P42Display.ClearScreen ( CH3,  0x00 );
	P42Display.PrintString ( CH0, " !\"#$%&\'()*+,-./0123456789", 0, 0, 0x08);
	P42Display.PrintString ( CH0, ":;<=>?@ABCDEFGHIJKLMNOPQRS", 0, 10, 0x08);
	P42Display.PrintString ( CH0, "TUVWXYZ[\\]^_`abcdefghijklm", 0, 20, 0x08);
	P42Display.PrintString ( CH0, "nopqrstuvwxyz{|}~",  0, 30, 0x08);

	PrintPaulString ( CH0, " !\"#$%&\'()*+,-./0123456789", 0, 40, 0x09 );
	PrintPaulString ( CH0, ":;<=>?@ABCDEFGHIJKLMNOPQRS", 0, 40+18, 0x0a );
	PrintPaulString ( CH0, "TUVWXYZ[\\]^_`abcdefghijklm", 0, 40+18*2, 0x0b );
	PrintPaulString ( CH0, "nopqrstuvwxyzéöíóñ", 0, 40+18*3, 0x0c );

	PrintPaulString ( CH1, " !\"#$%&\'()*+,-./0123456789", 0, 40, 0x19 );
	PrintPaulString ( CH1, ":;<=>?@ABCDEFGHIJKLMNOPQRS", 0, 40+18, 0x1a );
	PrintPaulString ( CH1, "TUVWXYZ[\\]^_`abcdefghijklm", 0, 40+18*2, 0x1b );
	PrintPaulString ( CH1, "nopqrstuvwxyzéöíóñ", 0, 40+18*3, 0x1c );

	PrintPaulString ( CH2, " !\"#$%&\'()*+,-./0123456789", 0, 40, 0x29 );
	PrintPaulString ( CH2, ":;<=>?@ABCDEFGHIJKLMNOPQRS", 0, 40+18, 0x2a );
	PrintPaulString ( CH2, "TUVWXYZ[\\]^_`abcdefghijklm", 0, 40+18*2, 0x2b );
	PrintPaulString ( CH2, "nopqrstuvwxyzéöíóñ", 0, 40+18*3, 0x0c );

	PrintPaulString ( CH3, " !\"#$%&\'()*+,-./0123456789", 0, 40, 0x49 );
	PrintPaulString ( CH3, ":;<=>?@ABCDEFGHIJKLMNOPQRS", 0, 40+18, 0x48 );
	PrintPaulString ( CH3, "TUVWXYZ[\\]^_`abcdefghijklm", 0, 40+18*2, 0x47 );
	PrintPaulString ( CH3, "nopqrstuvwxyzéöíóñ", 0, 40+18*3, 0x46 );


	Serial.println(F("End of test! [Restart press key]"));
	delay(1);
	while (Serial.available() == 0) {};
	incomingByte = Serial.read();
}
