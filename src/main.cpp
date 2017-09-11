#include <stdio.h>
#include <fcntl.h>  /* File Control Definitions		  */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions		 */
#include <time.h> /* UNIX Standard Definitions		 */
#include <errno.h>  /* ERROR Number Definitions		  */
#include <iostream>
#include <sstream>
#include <cstring>

#include "NMEAGPS.h"
#include "SerialLine.h"
#include "RPILCM1602.h"

#define DISP_TIME 3
#define MPH_THRESHOLD 0.75
#define AVG_STRENGTH 0.75

int main(int argc, char** args )
{
	// Serial device the GPS is connected to
	// TODO - Use a parameter for this
	char *device = "/dev/serial0";

	// Initialize the GPS module
	NMEAGPS gps = NMEAGPS(device);

	// Initialize the LCD
	RPILCM1602 lcd;
	lcd.setBacklight(0b100);
	lcd.command(CMD_DISP_ON);
	lcd.print("Connecting...", 0,0);
	
	// Total distance traveled while running
	float distance = 0;

	// Average speed
	float avgSpeed = 0;

	// Status values to change between showing LAT&LONG and distance traveled
	bool flip = false;
	bool flipChanged = false;
	int timer = DISP_TIME;

	// No connection screen
	bool showingNoConnection = true;

	// Main logic loop
	// TODO - Linked list updating objects based on system timer
	while(true)
	{
		// Update the GPS object
		gps.update();

		// Check for reception
		if(gps.fixStatus())
		{
			// Received new data
			if (gps.dataChange()) 
			{
				showingNoConnection = false;
				
				if (!flipChanged)
				{
					// Display GPS time and MPH
					lcd.print(gps.getTimeStr(),0 ,1);
					lcd.print(std::to_string(gps.getMPH()).substr(0,4), 9,1);
					lcd.print("MPH", 13,1);
				}else{
					lcd.print(std::to_string(avgSpeed).substr(0,4), 9,1);
					lcd.print("AVG", 13,1);
					flipChanged = false;
				}
				
				// If the MPH is greater than MPH_THRESHOLD add it to the distance
				if(gps.getMPH() > MPH_THRESHOLD)
					distance += gps.getMPH() / 3600;

				
				avgSpeed += (gps.getMPH() - avgSpeed)*AVG_STRENGTH;

				// Top row of LCD changes every DISP_FLIP gps.dataChange()'s
				// TODO - GPS will eventually update at 10Hz and this won't be able to rely on that 
				if(flip)
				{
					// Display LAT&LONG
					lcd.print(gps.getLatitude().substr(0,7), 0,0);
					lcd.print(",", 7,0);
					lcd.print(gps.getLongitude().substr(0,8), 8,0);
				}else{
					// Display miles traveled
					lcd.print("Miles: ", 0,0);
					lcd.print(std::to_string(distance), 7,0);
				}

				// Decrement timer and check if display should change
				timer--;				
				if(!timer)
				{
					flip = !flip;
					flipChanged = true;
					timer = DISP_TIME;
					
				}
				// Signal is valid so set backlight to "white"		
				lcd.setBacklight(0b111);

				
			}
		}else{
			if(!showingNoConnection)
			{
				showingNoConnection = true;
				// Signal is invalid so set backlight to red		
				lcd.setBacklight(0b100);
				lcd.command(CMD_CLEAR);
				lcd.print("Connection Lost", 0,0);
			}
		}
	
	}

	return 0;
}
