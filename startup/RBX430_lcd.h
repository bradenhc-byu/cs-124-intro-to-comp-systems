//******************************************************************************
//	RBX430_lcd.h
//
//  Author:			Paul Roper, Brigham Young University
//  Revisions:		1.0		01/25/2012	RBX430-1
//					1.1		11/25/2012	font attributes
//					1.2		03/22/2013	standalone typedef's
//
//******************************************************************************
#ifndef LCD_H_
#define LCD_H_

#ifndef RBX430_H_
#define RBX430_H_
typedef signed char int8;
typedef signed int int16;
typedef signed long int32;
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
#endif

//	initialization Sitronix ST7529 constants
#ifndef VOP_CODE
#define VOP_CODE	335				// vop = 14.0v
#endif
#define LCD_DELAY	50				// 50 ms

#define HD_X_MAX			160				// display size
#define HD_Y_MAX			160

#define CHAR_SIZE			8				// character height
#define FILL				0x10			// object fill bit

#define PRINT_BUFFER_SIZE	32				// print buffer size

enum {	SINGLE_OFF, SINGLE,					// 0/1
		TRIPLE_OFF, TRIPLE,					// 2/3
		PENT_OFF, PENT,						// 4/5
		DOUBLEX_OFF, DOUBLEX,				// 6/7
		TRIPLEX_OFF, TRIPLEX,				// 8/9
		QUADX_OFF, QUADX,					// 10/11
		PENTX_OFF, PENTX,					// 12/13
		SEXTX_OFF, SEXTX	};				// 14/15

//	lcd modes

#define LCD_PROPORTIONAL	0x01			// \p
#define LCD_REVERSE_FONT	0x02			// \c
#define LCD_2X_FONT			0x04			// \d
#define LCD_OR_CHAR			0x08			// \o

//	lcd prototypes
uint8 lcd_init(void);
void lcd_clear(void);
void lcd_set(uint16 value);
void lcd_backlight(uint8 backlight);
void lcd_volume(uint16 volume);

//	lcd character data
uint8 lcd_mode(int8 mode);
unsigned char lcd_putchar(unsigned char c);
uint8 lcd_printf(const char* fmt, ...);
uint8 lcd_cursor(int16 x, int16 y);

uint8 lcd_image(const uint8* image, int16 x, int16 y);
uint8 lcd_blank(int16 x, int16 y, uint16 width, uint16 height);

uint8 lcd_bitImage(const uint8* image, int16 x, int16 y, uint8 flag);
uint8 lcd_wordImage(const uint16* image, int16 x, int16 y, uint8 flag);
uint8 lcd_fillImage(int16 x, int16 y, uint16 width, uint16 height, uint8 flag);

uint16 lcd_read_word(int16 x, int16 y);
void lcd_write_word(int16 x, int16 y, uint16 data);

uint8 lcd_point(int16 x, int16 y, int8 flag);
void lcd_circle(int16 x, int16 y, uint16 radius, uint8 pen);
void lcd_square(int16 x, int16 y, uint16 side, uint8 pen);
void lcd_rectangle(int16 x, int16 y, uint16 w, uint16 h, uint8 pen);
void lcd_triangle(int16 x0, int16 y0, uint16 r0, uint8 pen);
void lcd_star(int16 x0, int16 y0, uint16 r0, uint8 pen);

unsigned divu3(unsigned n);

///-- deprecated-----------------------------------------------------------------
//#define lcd_display lcd_mode
//#define	READ_POINT		4
//#define M2B3P(P0,P1,P2)	((0xc0*P1|0x1f*P2)^0xff),((0Xf8*P0|0x07*P1)^0xff)
//#define lcd_image1	lcd_bitImage
//#define lcd_image2	lcd_wordImage
//#define LCD_FRAM_CHARACTER	0x10
//#define LCD_REVERSE_DISPLAY	0x20
//enum {SINGLE_PEN_OFF, SINGLE_PEN, DOUBLE_PEN_OFF, DOUBLE_PEN};

#endif /*LCD_H_*/
