#ifndef DISPLAY_PINDEFS_H
#define DISPLAY_PINDEFS_H

/* pindefs.h */
/* define the pins used in the SEA-DMI ESP504 conversion */

/* It may be reasonable to use the Arduino SPI library and       */
/* reassign MISO/MOSI and SCK pins to the hardware intended pins */

//test

const int A_0 = 4;        	//23 - PC5
const int A_1 = 3;        	//22 - PC6
const int A_2 = 2;        	//21 - PC7
const int SpiEn = 8;     	//26
const int CW = 10;      //clockwise switch -- 6
const int CCW = 9;    //counterclockwise switch -- 5
const int LockDet = 11;     //4
const int AdcCsOut = 12;    //8
const int LED = 13;
const int BEEP = 24;      //9
const int U3SW = 25;       //24
const int PttIn = 26;      //36

//moved to use hardware sck libraries
//except for SCKLOCAL -- that hardware-wise is 32 but decided
//to use the software SPI libs to relieve some wire banjo-stringing
const int SCKLOCAL = 5;        //33 moved from 13 to 8 as 13 has the led
const int MOSILOCAL = 0;       //32
const int MISOLOCAL = 1;       //31

//SSD1306 display 0x78
//using hardware i2c
//pin 19 for SCL0
//pin 18 for SDA0

#endif
