// VGA timing 
// written by
// Wolfgang Friedrich
// 4.Feb2019

// Based on vga.h and vga.c
// provided by VLSI Solutions http://www.vlsi.fi/en/home.html
// as part of demo code for the VS23S010D
// For more information see:
// http://www.vsdsp-forum.com/phpbb/viewforum.php?f=14
// http://www.vlsi.fi/en/products/vs23s010.html
// https://webstore.vlsi.fi/epages/vlsi.sf/en_GB/?ObjectID=13893093



#ifndef VGA_H
#define VGA_H

#define u_int16 unsigned int
#define u_int32 unsigned long

/// U & V data shift values in a 8-bit pixel are defined.
#define USHIFT 6
#define VSHIFT 4
#define UBITS 2
#define VBITS 2
#define YBITS 4



#define SYNC_LEVEL  0x0000
#define BLANK_LEVEL 0x0040

#define BLACK_LEVEL 0x0080
//#define BURST_LEVEL 0x4050
//#define BURST_LEVEL 0x6050
//#define BURST_LEVEL 0xc050
//#define BURST_LEVEL 0x8050
//#define BURST_LEVEL 0xB050
//#define BURST_LEVEL 0x0C50 //VUYY ntsc hok
//#define BURST_LEVEL 0x4C50 //VUYY ntsc hok
//#define BURST_LEVEL 0xC450 //VUYY ntsc hok //PALissa OK
//#define BURST_LEVEL 0xB050
//#define BURST_LEVEL 0x0850

//#define BURST_LEVEL 0xDD50 //VS23S 256 COLOR STRONG COLORS DD50
//#define BURST_LEVEL 0xbd80
#define BURST_LEVEL 0x9980
#define BORDER_LEVEL 0x2394
#define BK_INDEX 0x35


#define LINE_COUNT 261
//PAL#define LINELENPW 286
#define LINELENPW 230
#define PROTO0 0
#define PROTO1 (LINELENPW)
#define PROTO2 (LINELENPW*2)
#define PROTO3 (LINELENPW*3)
#define INDEX0 (LINELENPW*4)
//#define LSTART16  1312
//#define LSTART16  1600

#define INDEXSTART16 ((LINELENPW)*2)
#define LSTART16  ((LINELENPW*4)+625*2)
//PAL MEMORY CALCULATION:
//286*4+625*2 = 1144 + 1250 = 2394; 
//63142 words pixel memory left (126284 bytes)
//480 * 262 = 125760

//#define HICOLOR

#ifdef HICOLOR

#else

#endif
#define BYTES_PER_PIXEL (BITS_PER_PIXEL / 8)
//#define VISIBLE_PIXELS 519

//PAL//#define VISIBLE_PIXELS 472

//#define VISIBLE_PIXELS 340

//#define VISIBLE_PIXELS 402
//#define VISIBLE_LINES 195
//PAL//#define VISIBLE_LINES 260
//#define VISIBLE_LINES 200
//#define VISIBLE_LINES (126000/VISIBLE_PIXELS)


//#define FIRST_VISIBLE_LINE 28
//PAL//#define FIRST_VISIBLE_LINE 36

#define LINE_ADDR_16(a) ((LSTART16) + ((WORDS_PER_LINE) * (a)))
#define LINE_ADDR_8(a) (((LSTART16) + ((WORDS_PER_LINE) * (a)))*2)
#ifdef HICOLOR
#define LINE_ADDR(a) (LINE_ADDR_16(a))
#define PIXEL_ADDRESS(x,y) (LINE_ADDR(y) + ((x)))
#else
#define LINE_ADDR(a) (LINE_ADDR_8((u_int32)a))
#define PIXEL_ADDRESS(x,y) (LINE_ADDR(y) + (BYTES_PER_PIXEL*(x)))
#endif

#ifdef HICOLOR
#define SET_PIXEL(x,y,a) {SpiRamWriteWord(PIXEL_ADDRESS((x),(y)),(a));}
#else
#define SET_PIXEL(x,y,a) {SpiRamWriteByte(PIXEL_ADDRESS((x),(y)),(a));}
#endif


// Pattern generator microcode
// ---------------------------
// Bits 7:6  a=00|b=01|y=10|-=11
// Bits 5:3  n pick bits 1..8
// bits 2:0  shift 0..6
#define PICK_A (0<<6)
#define PICK_B (1<<6)
#define PICK_Y (2<<6)
#define PICK_NOTHING (3<<6)
#define PICK_BITS(a)(((a)-1)<<3)
#define SHIFT_BITS(a)(a)


//#ifdef HICOLOR			
//// 16 bits per pixel, U4 V4 Y8
//#define OP1 (PICK_B + PICK_BITS(4) + SHIFT_BITS(4))
//#define OP2 (PICK_A + PICK_BITS(4) + SHIFT_BITS(4))
//#define OP3 (PICK_Y + PICK_BITS(8) + SHIFT_BITS(4))
//#define OP4 (PICK_NOTHING + SHIFT_BITS(4))
//
//#else
// 8 bits per pixel, U2 V2 Y4
#define OP1 (PICK_Y + PICK_BITS(1) + SHIFT_BITS(1))
#define OP2 (PICK_B + PICK_BITS(1) + SHIFT_BITS(1))
#define OP3 (PICK_A + PICK_BITS(1) + SHIFT_BITS(1))
//#define OP2 (PICK_NOTHING)
//#define OP3 (PICK_NOTHING)
#define OP4 (PICK_NOTHING)
//#endif

// from vga.c

/// Line length in microseconds (float, observe accuracy)
//#define LINE_LENGTH_US 31.777557 
#define LINE_LENGTH_US 31.50 
//#define LINE_LENGTH_US 28.2 
/// Frame length in lines (visible lines + nonvisible lines)
#define TOTAL_LINES 525
//#define TOTAL_LINES 625
/// Number of visible lines in frame (excluding any border areas)
#define VISIBLE_LINES 480
/// Number of lines used for VSYNC
#define SYNC_LINES 2
/// Number of lines used after the VSYNC but before visible area
#define BACKPORCH_LINES 33

#define FRONT_PORCH_LINES 10

#define START_LINE SYNC_LINES + BACKPORCH_LINES


//#define CRYSTALVGA
#define CRYSTALPAL
//#define CRYSTALNTSC

#ifdef CRYSTALVGA
/// Crystal frequency in MHZ (float, observe accuracy)
#define XTAL_MHZ 12.000

#define HSYNC_WIDTH_XTALCLKS 46
#define BACKPORCH_BORDER_XTALCLKS 23
#define FRONTPORCH_BORDER_XTALCLKS 8

#endif

#ifdef CRYSTALPAL
/// Crystal frequency in MHZ (float, observe accuracy)
#define XTAL_MHZ 4.43361875

#define HSYNC_WIDTH_XTALCLKS 17
#define BACKPORCH_BORDER_XTALCLKS 8
#define FRONTPORCH_BORDER_XTALCLKS 3

#endif

#ifdef CRYSTALNTSC
/// Crystal frequency in MHZ (float, observe accuracy)
#define XTAL_MHZ 3.579545

#define HSYNC_WIDTH_XTALCLKS 14
#define BACKPORCH_BORDER_XTALCLKS 7
#define FRONTPORCH_BORDER_XTALCLKS 2

#endif


/// Number of visible pixels (excluding any border areas)
#define VISIBLE_PIXELS 640
/// Width, in PLL clocks (XTAL*8), of each pixel
#define PLLCLKS_PER_PIXEL 2
/// How many bits per pixel - this is used to calculate memory need
/// per line and should not conflict with the pattern generator microcode,
/// which actually controls how many bits the engine will pic per each pixel
#define BITS_PER_PIXEL 3

// Pattern generator microcode

/*
#define OP1 (PICK_B + PICK_BITS(4) + SHIFT_BITS(4))
#define OP2 (PICK_A + PICK_BITS(4) + SHIFT_BITS(4))
#define OP3 (PICK_Y + PICK_BITS(8) + SHIFT_BITS(4))
#define OP4 (PICK_NOTHING + SHIFT_BITS(4))


#define OP1 (PICK_Y + PICK_BITS(1) + SHIFT_BITS(1))
//#define OP2 (PICK_Y + PICK_BITS(4) + SHIFT_BITS(4))
#define OP2 (PICK_NOTHING + SHIFT_BITS(0))
#define OP3 (PICK_NOTHING + SHIFT_BITS(0))
#define OP4 (PICK_NOTHING + SHIFT_BITS(0))
*/


/// Words per one visible line of normal pixel data
#define WORDS_PER_LINE ((VISIBLE_PIXELS * BITS_PER_PIXEL + 15) / 16)

/// Reserve memory for this number of different prototype lines 
/// (prototype lines are used for sync timing, porch and border area)
#define PROTOLINES 4

/// Define USE_SLOTTED_PROTOS if you want to use fixed length prototype "slots" 
/// (allows patterned border area by defining different protolines 
/// also for visible lines in addition to nonvisible lines)
#define USE_SLOTTED_PROTOS

#ifdef USE_SLOTTED_PROTOS
/// Protoline lenght is one slot fixed in VS23S010 hardware: 512 bytes
/// (if your real protoline lenght is longer than one slot, you must use several slots per proto and there are total 16 slots)
#define PROTOLINE_LENGTH_WORDS 256
#else
/// Protoline lenght is the real lenght of protoline (optimal memory layout, but visible lines' prototype must always be proto 0)
#define PROTOLINE_LENGTH_WORDS ((LINE_US*XTAL_MHZ)+1)
#endif

#define FIRST_LINE (SYNC_LINES + FRONT_PORCH_LINES)
#define PLL_MHZ (XTAL_MHZ * 8.0)
#define PLLCLKS_PER_LINE ((u_int16)((LINE_LENGTH_US * PLL_MHZ)+0.5))
#define COLORCLKS_PER_LINE ((u_int16)((LINE_LENGTH_US * XTAL_MHZ)+0.5))
#define PROTO_AREA_WORDS (PROTOLINE_LENGTH_WORDS * PROTOLINES)
#define INDEX_START_LONGWORDS ((PROTO_AREA_WORDS+1)/2)
#define INDEX_START_WORDS (INDEX_START_LONGWORDS * 2)
#define INDEX_START_BYTES (INDEX_START_WORDS * 2)



#define DATA_START_WORDS (INDEX_START_WORDS + (TOTAL_LINES * 3 + 1) / 2)
#define LINE_WORD_ADDRESS(a) (DATA_START_WORDS + WORDS_PER_LINE * (a))

#define PICTURE_START_XTALCLKS (HSYNC_WIDTH_XTALCLKS + BACKPORCH_BORDER_XTALCLKS)
#define PICTURE_END_XTALCLKS (COLORCLKS_PER_LINE - FRONTPORCH_BORDER_XTALCLKS)

#define PROTOLINE_BYTE_ADDRESS(n) (PROTOLINE_LENGTH_WORDS*2*(n))
#define PROTOLINE_WORD_ADDRESS(n) (PROTOLINE_LENGTH_WORDS*(n))


#endif
