#ifndef RPILCM160_h
#define RPILCM160_h

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <cstring>
#include <fcntl.h>  /* File Control Definitions		  */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions		 */
#include <time.h> /* UNIX Standard Definitions		 */
#include <errno.h>  /* ERROR Number Definitions		  */
#include <chrono>
#include <thread>
#include <vector>

#include "RPIGPIO.h"

/*
 * Pin Mapping
 *
 * This is all software mapped and not currently reliant on any hardware 
 * functionality. Feel free to modify
 */

// Command Pins
#define RS 23
#define EN 24

// Data input pins
#define DATA_PIN_COUNT 4 // Change for 4 or 8 bit entry mode(only 4b now working)
#define DB0 0
#define DB1 0
#define DB2 0
#define DB3 0
#define DB4 5
#define DB5 6
#define DB6 13
#define DB7 19

// RGB Backlight pins
#define LR 17
#define LG 27
#define LB 22

/*
 * Display Information
 *
 * Details specific to the display being connected. Most cheap displays use the
 * HD44780U logic chip to drive them. This supports many physical display types.
 *
 * So this software, although writen for a 16x2, should be compatible with any
 * HD44780U display by modifying these values.
 */
#define CHAR_COL 16
#define CHAR_ROW 2

#define CHAR_BUFFER 80
#define LINE_BREAK_POS 0x40

/* 
 * Common Commands
 *
 * These are 8bit commands that the HD44780U chip takes. When in 4bit mode these
 * are parsed as nybles.
 */
#define CMD_CLEAR         0b00000001

#define CMD_FNSET_INIT    0b00110000
#define CMD_FNSET4_EN     0b00100000
#define CMD_FNSET4_SET    0b00101000

#define CMD_DISP_OFF      0b00001000
#define CMD_DISP_ON       0b00001100
#define CMD_DISP_CURON    0b00001110

#define CMD_ENTMO_LR      0b00000110

#define CMD_CURS_ON       0b00011100
#define CMD_CURS_OFF      0b00011000

#define CMD_MOVE          0b10000000

#define CHARSET_LEN 256
class RPILCM1602
{
public:
	RPILCM1602(); 
	~RPILCM1602();

	void command(int data);
	void command(int data, int msDelay);

	void print(std::string text, int pos, int line);
	void print(const char text[], int pos, int line, int length);
	
	void setBacklight(int blBits);

	void initDisplay();

private:
	void initPins();

	char LCM1602C_CHARSET[CHARSET_LEN];
	void buildCharset();
	
	void delay(int ms);

	std::vector<RPIGPIO*> dataInput;
	std::vector<RPIGPIO*> backlight;

	RPIGPIO enPin;
	RPIGPIO rsPin;

	bool mode4bit;
};

#endif
