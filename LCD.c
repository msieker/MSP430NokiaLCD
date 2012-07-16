/*
 * LCD.c
 *
 *  Created on: Jul 15, 2012
 *      Author: Matt
 */
#include <msp430.h>
#include "LCD.h"

#define SEND_BIT(mask) LCD_OUT &= ~LCD_CK; if((command & mask)) LCD_OUT |= LCD_MOSI; else LCD_OUT &= ~LCD_MOSI;LCD_OUT |= LCD_CK;
inline void LcdWriteCommand(unsigned char command){
	LCD_OUT &= ~LCD_CK;
	LCD_OUT &= ~LCD_MOSI;
	LCD_OUT |= LCD_CK;
	SEND_BIT(128)
	SEND_BIT(64)
	SEND_BIT(32)
	SEND_BIT(16)
	SEND_BIT(8)
	SEND_BIT(4)
	SEND_BIT(2)
	SEND_BIT(1)
}


inline void LcdWriteData(unsigned char command){
	LCD_OUT &= ~LCD_CK;
	LCD_OUT |= LCD_MOSI;
	LCD_OUT |= LCD_CK;
	SEND_BIT(128)
	SEND_BIT(64)
	SEND_BIT(32)
	SEND_BIT(16)
	SEND_BIT(8)
	SEND_BIT(4)
	SEND_BIT(2)
	SEND_BIT(1)
}

void LcdWrite(unsigned char command, const unsigned char* data, int length){
    unsigned int out=0;
    unsigned char i;
    out = command;
    LCD_OUT &= ~LCD_CS;
    LcdWriteCommand(out);

    for(i=0;i<length;i++){
    	LcdWriteData(data[i]);
    }
    LCD_OUT |= LCD_CS;
}

void LcdLoadLUT() {
	// 128-Bytes must be written to the LUT regardless of the color mode.
	// 4K-color (4-4-4) and 65K-color(5-6-5) data input are transferred 6(R)-6(G)-6(B) through RGB LUT table.
	LcdWriteCommand(LCD_P_RGBSET);
	LCD_OUT &= ~LCD_CS;
	char c;
	for (c = 0; c < 0x1F; c += 2) {
		LcdWriteData(c);
	}
	for (c = 0; c < 0x3F; c++) {
		LcdWriteData(c);
	}
	for (c = 0; c < 0x1F; c += 2) {
		LcdWriteData(c);
	}
	LCD_OUT |= LCD_CS;
}

void LcdInit(){
    LCD_OUT &= ~LCD_RES;
    __delay_cycles(100);
    LCD_OUT |= LCD_RES;
    __delay_cycles(100);

    static const unsigned char colmodData[] = {0x02};
    static const unsigned char madctlData[] = {0xC8};
    static const unsigned char setconData[] = {0x30};
    LcdWrite(LCD_P_SLEEPOUT,0,0);
    LcdWrite(LCD_P_COLMOD,colmodData,1);
    LcdWrite(LCD_P_MADCTL,madctlData,1);
    LcdWrite(LCD_P_SETCON,setconData,1);
    __delay_cycles(100);
    LcdWrite(LCD_P_DISPON,0,0);
    LcdLoadLUT();
}

void LcdClear(unsigned char color){
    unsigned int i;
    LCD_OUT &= ~LCD_CS;
    LcdWriteCommand(LCD_P_PASET);
    LcdWriteData(0);
    LcdWriteData(131);
    LcdWriteCommand(LCD_P_CASET);
	LcdWriteData(0);
	LcdWriteData(131);

    LcdWriteCommand(LCD_P_RAMWR);

    unsigned char color1 = (color>>4)&0xFF;
    unsigned char color2 = ((color&0x0F)<<4) | (color>>8);
    unsigned char color3 = color & 0xFF;
    for(i=131*131; i>0;i--){
    	LcdWriteData(color);
    }
    LCD_OUT |= LCD_CS;
}

void LcdSetXY(unsigned char x,unsigned char y){
	LCD_OUT &= ~LCD_CS;
	LcdWriteCommand(LCD_P_PASET);
	LcdWriteData(x);
	LcdWriteData(y);

	LcdWriteCommand(LCD_P_CASET);
	LcdWriteData(x);
	LcdWriteData(y);
	LCD_OUT |= LCD_CS;
}

void LcdDrawPixel(unsigned char x, unsigned char y, unsigned char color) {
	LcdSetXY(x, y);
	LCD_OUT &= ~LCD_CS;
	LcdWriteCommand(LCD_P_RAMWR);
	LcdWriteData(color);
	LcdWriteCommand(LCD_P_NOP);
	LCD_OUT |= LCD_CS;
}

void LcdDrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,unsigned char color) {
	int dy = y1 -y0;
	int dx = x1 -x0;
	int stepx, stepy;
	if (dy < 0) { dy = -dy; stepy = -1;} else {stepy =1; }
	if(dx < 0) {dx= -dx;  stepx = -1; } else {stepx = 1; }
	dy<<= 1;        // dyis now2*dy
	dx<<= 1;        // dxis now2*dx
	LcdDrawPixel(x0, y0, color);
	if(dx > dy) {
		int fraction= dy- (dx >> 1);  // same as2*dy -dx
		while(x0!=x1) {
			if(fraction >= 0){
				y0+=stepy;
				fraction -= dx;    // same asfraction -=2*dx
			}
			x0+=stepx;
			fraction+= dy;    // same asfraction -=2*dy
			LcdDrawPixel(x0, y0, color);
		}
	} else {
		int fraction= dx- (dy >> 1);
		while(y0!=y1) {
			if(fraction >= 0){
				x0+=stepx;
				fraction -= dy;
			}
			y0+=stepy;
			fraction+= dx;
			LcdDrawPixel(x0, y0, color);
		}
	}
}

void LcdDrawRect(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char fill,unsigned char color) {
	int  xmin, xmax, ymin, ymax;
	int  i;
	// check if the rectangle is to be filled
	if (fill == FILL) {
		// best way to create a filled rectangle is to define a drawing box
		// and loop two pixels at a time
		// calculate the min and max for x and y directions
		xmin = (x0 <=x1)? x0:x1;
		xmax = (x0 >x1) ? x0: x1;
		ymin = (y0 <=y1)? y0:y1;
		ymax = (y0 >y1) ? y0: y1;
		// specify the controller drawing box according to those limits
		// Row address set (command 0x2B)
		LCD_OUT &= ~LCD_CS;
		LcdWriteCommand(LCD_P_PASET);
		LcdWriteData(xmin);
		LcdWriteData(xmax);
		// Column address set(command 0x2A)
		LcdWriteCommand(LCD_P_CASET);
		LcdWriteData(ymin);
		LcdWriteData(ymax);
		// WRITE MEMORY
		LcdWriteCommand(LCD_P_RAMWR);
		// loop on total number of pixels/ 2
		int totalPixels=(ymax-ymin+1)*(xmax-xmin+1);
		for (i= 0; i < totalPixels; i++) {
			// use the color value to output three data bytes covering two pixels
			LcdWriteData(color);
		}
		LCD_OUT |= LCD_CS;
	} else {
		// best way to draw an unfilled rectangle is to draw four lines
		LcdDrawLine(x0, y0, x1, y0, color);
		LcdDrawLine(x0, y1, x1, y1, color);
		LcdDrawLine(x0, y0, x0, y1, color);
		LcdDrawLine(x1, y0, x1, y1, color);
	}
}
