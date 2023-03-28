// VS23S040D library file for 
// P42 Video Display Shield for Arduino
// Wolfgang Friedrich
// Last change: 2020/Jun/24
// https://hackaday.io/project/xxxxx-VGA-shield-wing
// https://github.com/wolfgangfriedrich/P42-VGA

#include "Arduino.h"
#include <SPI.h>

#ifdef VGA640x480
	#include "vga.h"
#endif

#include "VS23S040D-VGA.h"


// Config SPI for communication witht the VLSI VS23S010D-L chip
P42Display::P42Display()
{
	//Serial.begin(115200);
	//Serial.println("");
	//Serial.println(F("P42 Display Shield Test"));

		// Config pins
	pinMode(nWPPin, OUTPUT);
	digitalWrite(nWPPin, HIGH);
	pinMode(nHOLDPin, OUTPUT);
	digitalWrite(nHOLDPin, HIGH);

	// Config SPI interface
	pinMode(slaveSelectPin, OUTPUT);
	digitalWrite(slaveSelectPin, HIGH);
	pinMode(MemSelectPin, OUTPUT);
	digitalWrite(MemSelectPin, HIGH);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST) ; 

}


// ----------------------------------------------
// Read 8bit register and return value
// if the debug parameter is set to TRUE, the register value is printed in format
// SPI read adress: 0x0 : h 0x4 : l 0x24
// SPI read result 0x424
byte P42Display::SPIReadRegister(byte address, boolean debug = false)
{
	byte result = 0;

	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and return value via SPI:
	SPI.transfer( address );
	result = SPI.transfer(0x00);
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
	Serial.print("Read ");
	_printdebug(address,result);
}
	return (result);
}

// ----------------------------------------------
// Read 16bit register and return value
word P42Display::SPIReadRegister16(byte address, boolean debug = false)
{
	word result = 0;
    
	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	//  send in the address and return value via SPI:
	SPI.transfer( address );
	result = SPI.transfer16(0x0000);
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
	Serial.print(F("Read "));
	_printdebug(address,result);
}
	return (result);
}

// ----------------------------------------------
// Write 8bit register
void P42Display::SPIWriteRegister(byte address, byte value, boolean debug = false)
{
	byte value_hi =0;

	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and value via SPI:
	SPI.transfer( address );
	SPI.transfer( value );
//	SPI.transfer( byte( value & 0xFF));
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
	Serial.print(F("Write "));
	_printdebug(address,value);
}
	return; 
}

// ----------------------------------------------
// Write 16bit register
// hi data byte is sent first, lo data byte last.
void P42Display::SPIWriteRegister16(byte address, word value, boolean debug = false)
{
	byte value_hi =0;

	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and value via SPI:
	SPI.transfer( address );
	SPI.transfer( byte(value >> 8) );
	SPI.transfer( byte( value & 0xFF));
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
	Serial.print(F("Write "));
	_printdebug(address,value);
}
	return; 
}

// ----------------------------------------------
// Write 32bit register
// hi data byte is sent first, lo data byte last.
void P42Display::SPIWriteRegister32(byte address, u_int32 value, boolean debug = false)
{
	byte value_hi =0;

	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and value via SPI:
	SPI.transfer( address );
	SPI.transfer( byte( ((u_int32)value >> 24) & 0xFF));
	SPI.transfer( byte( ((u_int32)value >> 16) & 0xFF));
	SPI.transfer( byte( ((u_int32)value >>  8) & 0xFF));
	SPI.transfer( byte(  (u_int32)value        & 0xFF));
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
	Serial.print(F("Write "));
	_printdebug(address, (u_int32)value);
}
	return; 
}

// ----------------------------------------------
// Write 40bit(5 byte) register
// hi data byte is sent first, lo data byte last.
void P42Display::SPIWriteRegister40(byte address, word source, word target, byte control, boolean debug = false)
{
	byte value_hi =0;

	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and value via SPI:
	SPI.transfer( address );
	SPI.transfer( byte( (source >> 8) & 0xFF));
	SPI.transfer( byte( (source     ) & 0xFF));
	SPI.transfer( byte( (target >> 8) & 0xFF));
	SPI.transfer( byte(  target       & 0xFF));
	SPI.transfer(        control             );
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
	Serial.print(F("Write "));
	_printdebug(address, source);
	_printdebug(address, target);
	_printdebug(address, control);
}
	return; 
}

// ----------------------------------------------
// Protoline Generator
// used from VLSI Arduino Examples
//void P42Display::_protoline(byte channel, u_int16 line, u_int16 offset, u_int16 limit, u_int16 data) {
//	u_int16 i = 0;
//	u_int32 w = (PROTOLINE_WORD_ADDRESS(line) + offset);
//
//	if (offset == BLANKEND) i = BLANKEND;
//	for (; i<=limit; i++) SPIWriteWord( channel, (u_int32)w++, data, false);
//}

// ----------------------------------------------
// Debug output of SPI address and value
void P42Display::_printdebug(byte address, u_int32 value) {
	
	Serial.print(F("SPI address: 0x"));
	Serial.print(address,HEX);
	Serial.print(F(" : 0x"));
	Serial.println(value,HEX);
}


// --------------------------------------------------------------------------------------
// Hexdump Flash memory to UART
// --------------------------------------------------------------------------------------
void SPImemdump (unsigned long address, unsigned int bytes) {

	unsigned long i = 0;
	byte result =0;
	
	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x03 );					// read data
	SPI.transfer( (address >>16) &0xFF );	// start address
	SPI.transfer( (address >> 8) &0xFF );
	SPI.transfer( (address     ) &0xFF );
	for (i = 0; i < bytes; i++) {
		if ((i % 8) == 0) {
			if (i != 0)
				Serial.println (";");
			Serial.print(" 0x");
			Serial.print(address + i,HEX);
			Serial.print(": ");
		}
		result = SPI.transfer( 0x00 );
		Serial.print(" 0x");
		if (result <0x10)
			Serial.print ("0");
		Serial.print(result,HEX);
	}
    digitalWrite(MemSelectPin,HIGH); 
	Serial.println("<");
  

}


// --------------------------------------------------------------------------------------
// Hexdump Video memory to UART
// --------------------------------------------------------------------------------------
void VIDEOmemdump (byte channel, unsigned long address, unsigned int bytes) {

	unsigned long i = 0;
	byte result =0;
	
	address = address + channel*ch_block;
	
	for (i = 0; i < bytes; i++) {
		if ((i % 8) == 0) {
			if (i != 0)
				Serial.println (";");
			Serial.print(" 0x");
			Serial.print(address + i,HEX);
			Serial.print(": ");
		}
		digitalWrite(slaveSelectPin,LOW);
		SPI.transfer( ReadSRAM );				// read data
		SPI.transfer( ((address+i) >>16) &0xFF );	// start address
		SPI.transfer( ((address+i) >> 8) &0xFF );
		SPI.transfer( ((address+i)     ) &0xFF );
		result = SPI.transfer( 0x00 );
//		result = SPIReadByte( channel,  address+i );
		digitalWrite(slaveSelectPin,HIGH); 
		Serial.print(" 0x");
		if (result <0x10)
			Serial.print ("0");
		Serial.print(result,HEX);
	}
	Serial.println("<");
  

}


// ----------------------------------------------
// General Config for the Display Shield
// used from VLSI Arduino Examples
word P42Display::Config( byte channel )
{
	word result = 0;
	u_int32 i,j = 0;
	word w = 0;

	// test the 4 chip memory instances
	SPIWriteWord (CH0, 0, 0xcafe, false);
	result = SPIReadWord (CH0, 0, false);
	Serial.println(result,HEX);
	VIDEOmemdump (CH0, 0, 8);

	SPIWriteWord (CH1, 0x600, 0xbad0, false);
	result = SPIReadWord (CH1, 0x600, false);
	Serial.println(result,HEX);
	VIDEOmemdump (CH1, (0x600<<1),8);

	SPIWriteWord (CH2, 0x800, 0xbeef, false);
	result = SPIReadWord (CH2, 0x800, false);
	Serial.println(result,HEX);
	VIDEOmemdump (CH2, (0x800<<1),8);

	SPIWriteWord (CH3, 0, 0xaffe, false);
	result = SPIReadWord (CH3, 0, false);
	Serial.println(result,HEX);
	VIDEOmemdump (CH3, 0,8);

	
	// Read ID register to make sure the shield is connected.
	result = SPIReadRegister16( ReadDeviceID, false );
	if ((result & 0xff00) == 0x2B00) {

#ifdef SERIAL_DEBUG
		Serial.print(F("Found P42 VGA module ID: 0x"));
		Serial.println(result,HEX);
		Serial.print(F("Selected Channel: "));
		Serial.println(channel);
#endif

#ifdef VGA640x480 
	#ifdef SERIAL_DEBUG
		Serial.println(F("Resolution: VGA 640x480 3x3 bit"));
	#endif
#endif

		// enable the channel(s) that are handed in.
		SPIWriteRegister (WriteMultiICAccessControl, 0x0f ^ ( 1 << 0 ), false);
		SPIReadRegister(ReadMultiICAccessControl, true);
		// SPI mem address auto increment
		SPIWriteRegister (WriteStatus, 0x40, true);
		//Set GP0..3 as outputs
		SPIWriteRegister( WriteGPIOControl, PIO4Dir | PIO4Low | PIO5Dir | PIO5Low | PIO6Dir | PIO6Low | PIO7Dir | PIO7Low , true  );
		// 1. enable and select PLL
		SPIWriteRegister16 (WriteVideoDisplayControl1, PLLEnable | SelPLLCLK, true );
		// 2. Clear memory, could be more elegant with autoincrement
#ifdef SERIAL_DEBUG
		Serial.println(F("Clear memory"));
#endif
		for ( i=0; i < 65536; i++)
			SPIWriteWord (0, i, 0x0000, false);
		// 3. set length of complete line in PLL clocks
		SPIWriteRegister16 (WriteLineLength, PLLCLKS_PER_LINE | VGP_Output, true );
		// 4. Enable Video Display Controller, don't set video mode, set program length (same as pixel_width in PPL clocks) and linecount.
		SPIWriteRegister16 (WriteVideoDisplayControl2, Enable_Video | Program_Length | LineCount, true );
		// 5. mem location of line indexes
		SPIWriteRegister16 (WritePictureIndexStartAddress, INDEX_START_LONGWORDS, true );
		// 6. set all line indexes to point to protoline 0
	#ifdef SERIAL_DEBUG
		Serial.println(F("set line index to protoline 0"));
	#endif
		for ( i=0; i < TOTAL_LINES; i++) {
			SPIWriteByte (0, INDEX_START_BYTES + i*3    , 0x00, false);
			SPIWriteByte (0, INDEX_START_BYTES + i*3 + 1, PROTOLINE_WORD_ADDRESS(0), false);
			SPIWriteByte (0, INDEX_START_BYTES + i*3 + 2, PROTOLINE_WORD_ADDRESS(0)>>8, false);
		}
	// At this time, the chip will continuously output the proto line 0.
	// This protoline will become our most "normal" horizontal line.
	// For TV-Out, fill the line with black level (0x60 is ok), 
	// and insert a few pixels of sync level (0) and color burst to the beginning.
	// Note that the chip hardware adds black level to all nonproto areas so 
	// protolines and normal picture have different meaning for the same Y value.
	// In protolines, Y=0 is at sync level and in normal picture Y=0 is at black level.
	// Line must start with the sync because the hardware may anyway output a few cycles of sync
	// level at the beginning of line before the pixel data starts coming from the memory.

	// For VGA Out, insert a Horizontal Sync pulse to GP0 (LQFP pin 20)

	// In protolines, each pixel is 8 PLLCLKs, which in TV-out modes means one color 
	// subcarrier cycle. Each pixel has 16 bits (one word): UUUUVVVVYYYYYYYY.

		// Construct protoline 0
//		#define BLANK_LEVEL 0x0040 // We call protoline's black level a "blank" level.
#ifdef SERIAL_DEBUG
		Serial.println(F("Construct protoline 0"));
#endif
		w = PROTOLINE_WORD_ADDRESS(0); // Could be w=0 because proto 0 always starts at address 0
		for (i=0; i<=COLORCLKS_PER_LINE; i++) {
			SPIWriteWord(0, (u_int16)w, BLANK_LEVEL, false);
			result = SPIReadWord (0, (u_int16)w++, false);
		}
		// Use GP pins to set HSYNC start
		w = PROTOLINE_WORD_ADDRESS(0);
		SPIWriteWord(0, (u_int16)w, 0x8860, false);		//GP3=1(VSYNC), GP2=0(HSYNC)
		// Use GP pins to set  HSYNC end
		w = PROTOLINE_WORD_ADDRESS(0);
		SPIWriteWord(0, (u_int16)w+HSYNC_WIDTH_XTALCLKS, 0x8C60, false);


	// For testing purposes, make some interesting pattern to proto 0
//	w = PROTOLINE_WORD_ADDRESS(0)+40; 
//	for (i=0; i<=48; i++) {
//		//SpiRamWriteWord(w++, 0x77FF); //"Proto-maximum" green level + color carrier wave
//		SPIWriteWord(0, w+i, BLANK_LEVEL+ i*10, false); //moderate green level + a little bit color carrier
//	}

//	SPIWriteWord(20,0x00FF, false);
//	SPIWriteWord(PROTOLINE_WORD_ADDRESS(1), 0xAAAA, false);
//	// GP2, GP3 pins to make red+blue strip
//	
//	#define RED_BIT 0x0400
//	#define BLUE_BIT 0x0800	
//	SPIWriteWord(PROTOLINE_WORD_ADDRESS(0) + 26, 0x8C60 + BLUE_BIT, false);
//	SPIWriteWord(PROTOLINE_WORD_ADDRESS(0) + 30, 0x8C60 + RED_BIT + BLUE_BIT, false);
//	SPIWriteWord(PROTOLINE_WORD_ADDRESS(0) + 34, 0x8C60 + RED_BIT, false);
//	SPIWriteWord(PROTOLINE_WORD_ADDRESS(0) + 38, 0x8C60, false);

//  VIDEOmemdump ( CH0, 0, 0x100 );
  
  
		// Construct protoline 1. This is the VSYNC line
	#ifdef SERIAL_DEBUG
		Serial.println(F("Construct protoline 1"));
	#endif
		w = PROTOLINE_WORD_ADDRESS(2);
		for (i=0; i<=COLORCLKS_PER_LINE; i++) {
			SPIWriteWord(0, (u_int16)w++, BLANK_LEVEL, false);
		}
		w = PROTOLINE_WORD_ADDRESS(2);
		SPIWriteWord(0, (u_int16)w, 0x8060, false); //VSYNC + HSYNC start, it's important to have a stable HSYNC, at same location in each line
		w = PROTOLINE_WORD_ADDRESS(2)+HSYNC_WIDTH_XTALCLKS;
		SPIWriteWord(0, (u_int16)w, 0x8460,false); //HSYNC end, keep VSYNC at sync
		SPIWriteWord(0, (u_int16)w+1, 0x0090,false);
		// 7. Set first 2 lines of frame to point to VSYNC protoline
	#ifdef SERIAL_DEBUG
		Serial.println(F("First 2 lines point to VSYNC lines"));
	#endif
		// frame starts, lines 1 and 2 sync pulse 
		for (i=0; i<2; i++) {
			SPIWriteByte (0, INDEX_START_BYTES + i*3, 0x00, false);
			SPIWriteByte (0, INDEX_START_BYTES + i*3 + 1, PROTOLINE_WORD_ADDRESS(2), false);
			SPIWriteByte (0, INDEX_START_BYTES + i*3 + 2, PROTOLINE_WORD_ADDRESS(2)>>8, false);
		}
		// At this point, lines 0 and 1 are VSYNC lines and all other lines are proto0.
		// A blank picture is shown on the monitor 

// !!! Video processor 0 does not generate any image data at this point.
// video 0 could be used with the comparators to generate the SYNC signals, but those parts
// are not populated. The idea is to have the GPIOs free for the parallel memory access.
		
	
		// Now, let's start constructing the visible picture. 
		// Set horizontal start of visible picture (in colorclocks)
//		SPIWriteRegister16 (WritePictureStart, PICTURE_START_XTALCLKS, true );
//		SPIWriteRegister16 (WritePictureEnd, PICTURE_END_XTALCLKS, true );
		
		// microcode
//		SPIWriteRegister32 (WriteProgram, ((OP4 << 24) | (OP3 << 16) | (OP2 << 8) | (OP1)), true );

		// Set pic line indexes to point to protoline 0 and their individual picture line.
//	#ifdef SERIAL_DEBUG
//		Serial.println(F("Pic line index to protoline 0"));
//	#endif
		//for (i=0; i<ENDLINE-STARTLINE; i++) { 
//		for (i=50; i<60; i++) { 
//			SPIWriteByte (INDEX_START_BYTES + i *3,      LINE_WORD_ADDRESS(0), false);
//			SPIWriteByte (INDEX_START_BYTES + i *3 + 1,  LINE_WORD_ADDRESS(0), false);
//			SPIWriteByte (INDEX_START_BYTES + i *3 + 2,  LINE_WORD_ADDRESS(0), false);
//		}
		
		Serial.print (F("Protos at "));
		for (i=0; i<4; i++) {
			Serial.print (F(" 0x") );
			Serial.print (PROTOLINE_WORD_ADDRESS(i), HEX);
		}
		Serial.println();
		Serial.print(F("INDEX16 AT 0x"));
		Serial.print(INDEX_START_WORDS, HEX);
		Serial.println();
		Serial.print(F("LINES16 AT "));
		for (i=0; i<4; i++) {
			Serial.print (F(" 0x") );
			Serial.print(LINE_WORD_ADDRESS(i),HEX);
		}
		Serial.println();

		// Now, let's start constructing the visible picture with the other 3 video processors. 
		// enable the channel(s) that are handed in.
		
		// for testing use channel 1 only
		
		SPIWriteRegister (WriteMultiICAccessControl, 0x0f ^ ( 1 << 1 ), false);
		SPIReadRegister(ReadMultiICAccessControl, true);
		// SPI mem address auto increment
		SPIWriteRegister (WriteStatus, 0x40, true);
		// 1. enable and select PLL
		SPIWriteRegister16 (WriteVideoDisplayControl1, PLLEnable | SelPLLCLK, true );
		// 2. Clear memory, could be more elegant with autoincrement
#ifdef SERIAL_DEBUG
		Serial.println(F("Clear colour memory"));
#endif
		for ( i=0; i < 65536; i++) {
//			SPIWriteWord (1, i, 0x0000, false);
//			SPIWriteWord (2, i, 0x0000, false);
			SPIWriteWord (3, i, 0x0000, false);
		}
		// 3. set length of complete line in PLL clocks
		SPIWriteRegister16 (WriteLineLength, PLLCLKS_PER_LINE | VGP_Output, true );
		// 4. Enable Video Display Controller, don't set video mode, set program length (same as pixel_width in PPL clocks) and linecount.
		SPIWriteRegister16 (WriteVideoDisplayControl2, Enable_Video | Program_Length | LineCount, true );
		// 5. mem location of line indexes
		SPIWriteRegister16 (WritePictureIndexStartAddress, INDEX_START_LONGWORDS, true );
		// 6. set all line indexes to point to protoline 0
	#ifdef SERIAL_DEBUG
		Serial.println(F("set line index to colour protoline 0"));
	#endif
		for ( i=0; i < TOTAL_LINES; i++) {
			SPIWriteByte (1, INDEX_START_BYTES + i*3    , 0x00, false);
			SPIWriteByte (1, INDEX_START_BYTES + i*3 + 1, PROTOLINE_WORD_ADDRESS(0), false);
			SPIWriteByte (1, INDEX_START_BYTES + i*3 + 2, PROTOLINE_WORD_ADDRESS(0)>>8, false);
//			SPIWriteByte (2, INDEX_START_BYTES + i*3    , 0x00, false);
//			SPIWriteByte (2, INDEX_START_BYTES + i*3 + 1, PROTOLINE_WORD_ADDRESS(0), false);
//			SPIWriteByte (2, INDEX_START_BYTES + i*3 + 2, PROTOLINE_WORD_ADDRESS(0)>>8, false);
//			SPIWriteByte (3, INDEX_START_BYTES + i*3    , 0x00, false);
//			SPIWriteByte (3, INDEX_START_BYTES + i*3 + 1, PROTOLINE_WORD_ADDRESS(0), false);
//			SPIWriteByte (3, INDEX_START_BYTES + i*3 + 2, PROTOLINE_WORD_ADDRESS(0)>>8, false);
		}
	// At this time, the chip will continuously output the proto line 0.

		// Construct protoline 0
#ifdef SERIAL_DEBUG
		Serial.println(F("Construct colour protoline 0"));
#endif
		w = PROTOLINE_WORD_ADDRESS(0); // Could be w=0 because proto 0 always starts at address 0
		for (i=0; i<=COLORCLKS_PER_LINE; i++) {
			SPIWriteWord(1, (u_int16)w, SYNC_LEVEL, false);
			SPIWriteWord(2, (u_int16)w, SYNC_LEVEL, false);
			SPIWriteWord(3, (u_int16)w++, BLANK_LEVEL, false);
		}
		// For green, add sync pulse
// no sync on green for now
//		w = PROTOLINE_WORD_ADDRESS(0);
//		for (i=0; i<=HSYNC_WIDTH_XTALCLKS; i++) {
//			SPIWriteWord(2, (u_int16)w++, SYNC_LEVEL, false);
//		}
		// Construct protoline 1 for green. This is the VSYNC line
#ifdef SERIAL_DEBUG
//		Serial.println(F("Construct green protoline 1"));
#endif
//		w = PROTOLINE_WORD_ADDRESS(2); // Could be w=0 because proto 0 always starts at address 0
//		for (i=0; i<=COLORCLKS_PER_LINE; i++) {
//			SPIWriteWord(2, (u_int16)w++, SYNC_LEVEL, false);
//		}
		// Set first 2 lines of frame to point to VSYNC protoline
	#ifdef SERIAL_DEBUG
//		Serial.println(F("First 2 green lines point to VSYNC lines"));
	#endif
		// frame starts, lines 1 and 2 sync pulse 
//		for (i=0; i<2; i++) {
//			SPIWriteByte (2, INDEX_START_BYTES + i*3, 0x00, false);
//			SPIWriteByte (2, INDEX_START_BYTES + i*3 + 1, PROTOLINE_WORD_ADDRESS(2), false);
//			SPIWriteByte (2, INDEX_START_BYTES + i*3 + 2, PROTOLINE_WORD_ADDRESS(2)>>8, false);
//		}
		// It seems to be 1 HSYNC pulse too long at the end, we'll see if that breaks the sync on green mode. 

		// Now, let's start constructing the visible picture. 
		// Set horizontal start of visible picture (in colorclocks)
		SPIWriteRegister16 ( WritePictureStart, PICTURE_START_XTALCLKS, true );
		SPIWriteRegister16 ( WritePictureEnd, PICTURE_END_XTALCLKS, true );
		// microcode
		SPIWriteRegister32 ( WriteProgram, ((OP4 << 24) | (OP3 << 16) | (OP2 << 8) | (OP1)), true );

		// Set pic line indexes to point to protoline 0 and their individual picture line.
	#ifdef SERIAL_DEBUG
		Serial.println(F("Pic line index to protoline 0"));
	#endif
		//for (i=0; i<ENDLINE-STARTLINE; i++) { 
		
		//
		for (i=START_LINE; i<START_LINE+40; i++) { 
			SPIWriteByte (1, INDEX_START_BYTES + i *3,      0x00, false);
			SPIWriteByte (1, INDEX_START_BYTES + i *3 + 1,  LINE_WORD_ADDRESS(i), false);
			SPIWriteByte (1, INDEX_START_BYTES + i *3 + 2,  LINE_WORD_ADDRESS(i)>>8, false);
		}
			for (i=START_LINE+40; i<START_LINE+80; i++) { 
			SPIWriteByte (2, INDEX_START_BYTES + i *3,      0x00, false);
			SPIWriteByte (2, INDEX_START_BYTES + i *3 + 1,  LINE_WORD_ADDRESS(i), false);
			SPIWriteByte (2, INDEX_START_BYTES + i *3 + 2,  LINE_WORD_ADDRESS(i)>>8, false);
		}
			for (i=START_LINE+80; i<START_LINE+120; i++) { 
			SPIWriteByte (3, INDEX_START_BYTES + i *3,      0x00, false);
			SPIWriteByte (3, INDEX_START_BYTES + i *3 + 1,  LINE_WORD_ADDRESS(i), false);
			SPIWriteByte (3, INDEX_START_BYTES + i *3 + 2,  LINE_WORD_ADDRESS(i)>>8, false);
		}

// Debug
VIDEOmemdump (CH0, PROTOLINE_WORD_ADDRESS(0), 32);
Serial.println();
VIDEOmemdump (CH0, PROTOLINE_WORD_ADDRESS(2)<<1, 32);
Serial.println();
VIDEOmemdump (CH0, INDEX_START_BYTES, 32);
Serial.println();
VIDEOmemdump (CH0, LINE_WORD_ADDRESS (0)<<1, 16);
Serial.println();

VIDEOmemdump ( CH1, PROTOLINE_WORD_ADDRESS (0), 32);
Serial.println();
VIDEOmemdump ( CH1, PROTOLINE_WORD_ADDRESS (2)<<1, 32);
Serial.println();
VIDEOmemdump ( CH1, INDEX_START_BYTES, 300);
Serial.println();
VIDEOmemdump ( CH1, LINE_WORD_ADDRESS (0)<<1, 16);
Serial.println();

VIDEOmemdump ( CH2, PROTOLINE_WORD_ADDRESS (0), 32);
Serial.println();
VIDEOmemdump ( CH2, PROTOLINE_WORD_ADDRESS (2)<<1, 32);
Serial.println();
VIDEOmemdump ( CH2, INDEX_START_BYTES, 32);
Serial.println();
VIDEOmemdump ( CH2, LINE_WORD_ADDRESS (0)<<1, 16);
Serial.println();

VIDEOmemdump ( CH3, PROTOLINE_WORD_ADDRESS (0), 32);
Serial.println();
VIDEOmemdump ( CH3, PROTOLINE_WORD_ADDRESS (2)<<1, 32);
Serial.println();
VIDEOmemdump ( CH3, INDEX_START_BYTES, 32);
Serial.println();
VIDEOmemdump ( CH3, LINE_WORD_ADDRESS (0)<<1, 16);
Serial.println();


		}
	else {
	#ifdef SERIAL_DEBUG
		Serial.print(F("No VS23S040D module found: 0x"));
		Serial.println(result,HEX);
	#endif
	}
		
	return result;
}

// ----------------------------------------------
// Read 8bit byte from SRAM memory
byte P42Display::SPIReadByte ( byte channel, u_int32 address)
{
	byte result = 0;
	byte result_hi = 0;
	byte result_lo = 0;
	
	address = address + (channel * ch_block);
	
	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and return value via SPI:
	SPI.transfer( ReadSRAM );
	SPI.transfer( byte ((address >> 16) & 0xFF ));
	SPI.transfer( byte ((address >>  8) & 0xFF ));
	SPI.transfer( byte( address         & 0xFF ));
	result = SPI.transfer( byte(0x00) );
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

	return result;
}
// ----------------------------------------------
// Read 16bit word from SRAM memory
word P42Display::SPIReadWord ( byte channel, u_int32 address, boolean debug = false)
{
	word result = 0;
	byte result_hi = 0;
	byte result_lo = 0;
	
	address = address << 1;
	address = address + (channel * ch_block);
 	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	//  send in the address and return value via SPI:
	SPI.transfer( ReadSRAM );
	SPI.transfer( byte ((address >> 16) & 0xFF ));
	SPI.transfer( byte ((address >>  8) & 0xFF ));
	SPI.transfer( byte( address         & 0xFF ));
	result_hi = SPI.transfer(0x00);
	result_lo = SPI.transfer(0x00);

	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

if (debug ){
	Serial.print(F("SPIReadWord Adr: 0x"));
	Serial.print(address,HEX);
	Serial.print(F(" value hi: 0x"));
	Serial.print(result_hi,HEX);
	Serial.print(F(" lo: 0x"));
	Serial.println(result_lo,HEX);
}
	result = (result_hi	<< 8) | (result_lo) ;
	return result;
}

// ----------------------------------------------
// Write 8bit byte into SRAM memory
void P42Display::SPIWriteByte ( byte channel, u_int32 address, byte value, boolean debug = false)
{
	address = address + (channel * ch_block);
	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and return value via SPI:
	SPI.transfer( WriteSRAM );
	SPI.transfer( byte ((address >> 16) & 0xFF ));
	SPI.transfer( byte ((address >>  8) & 0xFF ));
	SPI.transfer( byte ( address        & 0xFF ));
	SPI.transfer( byte ( value          & 0xFF ));
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

	if (debug ){
		Serial.print(F("SPI opcode: 0x"));
		Serial.print(WriteSRAM,HEX);
		Serial.print(F(" adress: 0x"));
		Serial.print(address,HEX);
		Serial.print(F(" : 0x"));
		Serial.println(value,HEX);
	};
	
}

// ----------------------------------------------
// Write 16bit word into SRAM memory
void P42Display::SPIWriteWord ( byte channel, u_int32 address, word value, boolean debug = false)
{
	byte high = value >> 8;
	byte low = value & 0xFF;
// print 1st because address gets shifted
	if (debug ){
		Serial.print(F("SPI opcode: 0x"));
		Serial.print(WriteSRAM,HEX);
		Serial.print(F(" adress: 0x"));
		Serial.print(address,HEX);
		Serial.print(F(" : 0x"));
		Serial.println(value,HEX);
//		Serial.print(F("SPIWriteWord value hi: 0x"));
//		Serial.print(byte ((value >> 8) & 0xFF),HEX);
//		Serial.print(F(" lo: 0x"));
//		Serial.println(byte (value) & 0xFF,HEX);
	};

	address = address << 1;
	address = ( address + (channel * ch_block));
	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and return value via SPI:
	SPI.transfer( WriteSRAM );
	SPI.transfer( byte ((address >> 16) & 0xFF ));
	SPI.transfer( byte ((address >>  8) & 0xFF ));
	SPI.transfer( byte ( address        & 0xFF ));
	SPI.transfer( high);
	SPI.transfer( low );
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

	
}

// ----------------------------------------------
// Draws a filled rectangle, with the specified color, from coordinates (x1,y1) to (x2,y2)
// used from VLSI Arduino demo
void P42Display::FilledRectangle ( byte channel, u_int16 x1, u_int16 y1, u_int16 x2, u_int16 y2, u_int16 color) {
	u_int16 width, i;
	u_int32 address;

	if (y1 >= VISIBLE_LINES) return; // If starting y-coordinate is past the picture borders, nothing is done
	if (y2 >= VISIBLE_LINES) y2=VISIBLE_LINES-1; // The rectangle stops at the bottom border of the image

	width = (x2-x1)+1;
	if (width>VISIBLE_PIXELS) width=VISIBLE_PIXELS;

	// Loops through one horizontal line at a time.
	while (y1 <= y2) {
		address = LINE_ADDR_8(y1) + x1;
		// Color values of each x coordinate of the horizontal line
		for (i=0; i<width; i++) {
			SPIWriteByte (channel, address+i, color, false);
		}  
		y1++;
	}
}

// -----------------------------------------------
// Fill memory locations of display data with colour, 0x00 would equal black 
void P42Display::ClearScreen (byte channel, byte colour) {
	u_int32 address;
	u_int32 length, i;

	// fast version
	// memory start and size
	address = ((u_int32)LINE_ADDR_8(0) + 0);
	//	length = (u_int32)VISIBLE_PIXELS * (u_int32)VISIBLE_LINES;
	length = ((u_int32)LINE_ADDR_8(VISIBLE_LINES-1) + VISIBLE_PIXELS-1)-address;
	address = address + (channel * ch_block);
	// take the SS pin low to select the chip:
	digitalWrite(slaveSelectPin,LOW);
	// send in the address and return value via SPI:
	SPI.transfer( WriteSRAM );
	SPI.transfer( byte ((address >> 16) & 0xFF ));
	SPI.transfer( byte ((address >>  8) & 0xFF ));
	SPI.transfer( byte ( address        & 0xFF ));
	for (i = 0; i < length; i++)
		SPI.transfer( colour );
	// take the SS pin high to de-select the chip:
	digitalWrite(slaveSelectPin,HIGH); 

}

// -----------------------------------------------
// Convert a RGB color value into VUY and set the pixel. 
// This only works with the video high color palette
// color is 0x00rrggbb
void P42Display::SetRGBPixel (byte channel, word x, word y, u_int32 colour) {
	
	u_int32 address;
	u_int16 red, green, blue;
	int Y, U, V;
	byte pixdata;
	
	if ((x <= VISIBLE_PIXELS-1) && (y <= VISIBLE_LINES-1)) {
		address = LINE_ADDR_8(y) + x;
		red = (colour >>16) & 0xFF;
		green = (colour >>8) & 0xFF;
		blue = colour & 0xFF;

		Y = (76 * red + 150 * green + 29 * blue) >> 8;
		U = (red << 7 - 107 * green - 20 * blue) >> 8;
		V = (-43 * red - 84 * green + blue << 7) >> 8;

		pixdata = 	((U>>(8-UBITS))<<USHIFT) |
					((V>>(8-VBITS))<<VSHIFT) |
					((Y>>(8-YBITS)));

		SPIWriteByte (channel, address, pixdata, false);
	}
}

// -----------------------------------------------
// Set a pixel with the given value interpreted as VUY
// color value is a word lower bits are used for lo colour palettes
void P42Display::SetYUVPixel (byte channel, word x, word y, byte colour) {
	
	u_int32 address;
	
	if ((x <= VISIBLE_PIXELS-1) && (y <= VISIBLE_LINES-1)) {
		address = LINE_ADDR_8(y) + x;
	
		SPIWriteByte (channel, address, colour, false);
//		SPIWriteByte (channel, address+1, colour, false);
	}
}


// -----------------------------------------------
// Print Char from FLASH
void P42Display::PrintChar (byte channel, char Letter, word x, word y, byte colour) {

byte bitmap[8];
u_int16 i,j = 0;
u_int32 address, char_address;
	
	address = LINE_ADDR_8(y) + x;
	char_address = (Letter - '!') * 8;

//		Serial.print(F(" 0x"));
//		Serial.println(char_address, HEX);

	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x03 );								// read data
	SPI.transfer( byte((char_address >>16) & 0xFF) );	// start address
	SPI.transfer( byte((char_address >> 8) & 0xFF) );
	SPI.transfer( byte( char_address       & 0xFF) );
	for (i = 0; i <= 7; i++)
		bitmap[i] = SPI.transfer( 0x00 );
	digitalWrite(MemSelectPin,HIGH); 

	for (i = 0; i <= 7; i++)
	{
//		Serial.print(F(" 0x"));
//		Serial.print(bitmap[i], HEX);
		
		for (j = 8; j > 0; j--)
		{
//			Serial.print(j);
			if ((bitmap[i] >> (j-1) ) & 0x01 ) {
				SetYUVPixel ( channel, x+8-j, y+i, colour);
//				Serial.print(F("."));
			}
		}	
	}
//	Serial.println();

}



// -----------------------------------------------
// Print String
void P42Display::PrintString (byte channel, char* Text, word x, word y, byte colour) {
char Letter;
u_int16 i,j = 0;
u_int16 x_loc = 0;

	Letter = Text[i];
	x_loc = x;
//	Serial.print (Letter);
//	Serial.print (Text[i]);
	
	while (Text[i] != 0x00) {
//		Serial.print (F('<'));
		if ((Text[i] >= '!') && (Text[i] <= '~')){			// char between 0x21 and 0x7F
//			Serial.print (x_loc, DEC);
//			Serial.print (F('<'));
			PrintChar (channel, Text[i], x_loc, y, colour);
		}
		x_loc = x_loc + 8;
		i++;
	}
//	Serial.println();
}

// -----------------------------------------------
// receive number of bytes on UART and write to Flash memory address
byte P42Display::UARTDataToFlash ( u_int32 length, u_int32 mem_location) {
	u_int32	i,j = 0;
	byte return_value = 0;
	
	Serial.print (F("Expecting"));
	Serial.print (length, DEC);
	Serial.print (F("= 0x"));
	Serial.print (length, HEX);
	Serial.print (F(" Bytes."));
	
	
	return return_value;
};   








// -----------------------------------------------
// Read Data from Flash, interpret as an BMP image and display at coordinates x,y
// Supported BMP: 24bit colour depth
// Non colour mapping yet
void P42Display::DisplayBMPFromFlash ( u_int32 mem_location, u_int16 x, u_int16 y ) {
	
	unsigned long counter = 0;
	byte inbyte = 0;
	byte pixelvalue = 0;
	byte red,green,blue = 0;
	
	unsigned long FileSize = 54;		// set to header size
	unsigned long PixelOffset = 54;		// Offset from beginning of file to the beginning of the bitmap data
	unsigned long Width = 0;			// 
	unsigned long Height = 0;			// 
	byte BitsPerPixel = 0;
	byte Compression = 0;
	unsigned long ulongtemp = 0;		// 


	SPImemdump (0, 16);
	SPImemdump (0x1000, 64);
	
	SPImemdump (mem_location + counter, 16);

	while ( counter < FileSize ) {
		
		digitalWrite(MemSelectPin,LOW);
		SPI.transfer( 0x03 );									// read data
		SPI.transfer( byte(((mem_location + counter) >>16) & 0xFF) );	// Flash start address
		SPI.transfer( byte(((mem_location + counter) >> 8) & 0xFF) );
		SPI.transfer( byte( (mem_location + counter)       & 0xFF) );
		inbyte = SPI.transfer( 0x00 );
		digitalWrite(MemSelectPin,HIGH); 
		
		
//Serial.print("(0x");
//Serial.print(mem_location + counter,HEX);
//Serial.print(")");
//Serial.print(inbyte,HEX);
//Serial.print(".");
		switch ( counter ) {
			case 0: {
				if (inbyte != 'B') {
					Serial.println(F("B0: Not a BMP"));
					counter = FileSize;						// to stop while loop
				}
				break;
			}
			case 1: {
				if (inbyte != 'M') {
					Serial.println(F("B1: Not a BMP"));
					counter = FileSize;						// to stop while loop
				}
				break;
			}
			case 2:{							// 2-5 file size in bytes
				ulongtemp = inbyte;
				break;
			}
			case 3:{
				ulongtemp += (inbyte<<8);
				break;
			}
			case 4:{
				ulongtemp += (inbyte<<16);
				break;
			}
			case 5:{
				FileSize = ulongtemp + (inbyte<<24);
				Serial.print(F("Filesize: "));
				Serial.println(FileSize);
				break;
			}
			case 0xA:{							// 10-13 pixel offset
				PixelOffset = inbyte;
				break;
			}
			case 0xB:{
				PixelOffset += (inbyte<<8);
				break;
			}
			case 0xC:{
				PixelOffset += (inbyte<<16);
				break;
			}
			case 0xD:{
				PixelOffset = PixelOffset + (inbyte<<24);
				Serial.print(F("PixelOffset: 0x"));
				Serial.println(PixelOffset,HEX);
				break;
			}
			case 0x12:{							// 0x12-0x15 width
				Width = inbyte;
				break;
			}
			case 0x13:{
				Width += (inbyte<<8);
				break;
			}
			case 0x14:{
				Width += (inbyte<<16);
				break;
			}
			case 0x15:{
				Width = Width + (inbyte<<24);
				break;
			}
			case 0x16:{							// 0x12-0x15 Height
				Height = inbyte;
				break;
			}
			case 0x17:{
				Height += (inbyte<<8);
				break;
			}
			case 0x18:{
				Height += (inbyte<<16);
				break;
			}
			case 0x19:{
				Height = Height + (inbyte<<24);
				break;
			}
			case 0x1C:{
				BitsPerPixel = inbyte;
				Serial.print(BitsPerPixel);
				Serial.println(F("bit image"));
				break;
			}
			case 0x1E:{
				Compression = inbyte;
				break;
			}
			default: {
				break;
			}
		}	// switch
		if ( counter >= PixelOffset ) {
			switch ( BitsPerPixel ) {
				case 8: {
					//Serial.println(F("8bit image"));
					digitalWrite(MemSelectPin,LOW);
					SPI.transfer( 0x03 );									// read data
					SPI.transfer( byte(((mem_location + 0x36 +inbyte*4) >>16) & 0xFF) );	// Flash start address of BMP colour table
					SPI.transfer( byte(((mem_location + 0x36 +inbyte*4) >> 8) & 0xFF) );
					SPI.transfer( byte( (mem_location + 0x36 +inbyte*4)       & 0xFF) );
					red = SPI.transfer( 0x00 );
					green = SPI.transfer( 0x00 );
					blue = SPI.transfer( 0x00 );
					digitalWrite(MemSelectPin,HIGH); 
					
					pixelvalue = (red>>6) + (green>>6) + (blue>>6);		// translate RGB into 12 value grey scale
					SetYUVPixel ( 0,x + (counter-PixelOffset)%Width , y+Height - (counter-PixelOffset)/Width, pixelvalue+2); // shift 2 values up
					
					break;
				}
				case 24: {
					//Serial.println(F("24bit image"));
					switch ( (counter-PixelOffset)%3 ) {
						// scale 24bit into 4 lower bit of pixelvalue for a 12-step grey scale image.
						case 0:{
							// blue sub-pixel
							pixelvalue = inbyte>>6;
							break;
						}
						case 1:{
							// green sub-pixel
							pixelvalue = pixelvalue + (inbyte>>6);
							break;
						}
						case 2:{
							// red sub-pixel
							pixelvalue = pixelvalue + (inbyte>>6);
							SetYUVPixel ( 0, x + (counter-PixelOffset)%Width , y+Height - (counter-PixelOffset)/Width, pixelvalue+2); // shift 2 values up
							break;
						}
						default: {
							break;
						}
					}
				}
				default: {
					break;
				}
			}
		}
		counter ++; // this is the last instruction in the file read while loop
	}		// while
	Serial.println (F("Image done."));




}