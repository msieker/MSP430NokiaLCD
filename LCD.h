/*
 * LCD.h
 *
 *  Created on: Jul 15, 2012
 *      Author: Matt
 */

#ifndef LCD_H_
#define LCD_H_

#define LCD_P_NOP		0x00
#define LCD_P_SLEEPOUT	0x11
#define LCD_P_INVON		0x21
#define LCD_P_COLMOD	0x3A
#define LCD_P_MADCTL	0x36
#define LCD_P_SETCON	0x25
#define	LCD_P_DISPON	0x29
#define LCD_P_RAMWR		0x2C
#define LCD_P_CASET		0x2A
#define LCD_P_PASET		0x2B
#define LCD_P_RGBSET    0x2D

#define		LCD_OUT		P2OUT
#define		LCD_DIR		P2DIR
#define		LCD_RES		BIT0
#define		LCD_CS		BIT1
#define		LCD_CK		BIT2
#define		LCD_MOSI	BIT3

#define		LCD_WIDTH	131
#define		LCD_HEIGHT	131

#define 	NOFILL		0
#define 	FILL    	1

#define		PIXELCOUNT	(131*132)/8

inline void LcdWriteCommand(unsigned char command);
inline void LcdWriteData(unsigned char command);
void LcdWrite(unsigned char command, const unsigned char* data, int length);
void LcdInit();
void LcdClear(unsigned char color);

void LcdSetXY(unsigned char x, unsigned char y);
void LcdDrawPixel(unsigned char x, unsigned char y, unsigned char color);
void LcdDrawRect(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char fill,unsigned char color);
void LcdDrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,unsigned char color);
#endif /* LCD_H_ */
