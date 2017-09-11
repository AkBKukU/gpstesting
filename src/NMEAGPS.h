#ifndef NMEAGPS_h
#define NMEAGPS_h 

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  /* File Control Definitions		  */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions		 */
#include <errno.h>  /* ERROR Number Definitions		  */
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <time.h>

#include "SerialLine.h"

class NMEAGPS
{

	public:
		NMEAGPS(char device[]);
		//~NMEAGPS();

		std::string getLatitude();
		std::string getLongitude();
		time_t getTime();
		std::string getTimeStr();
		float getKnots();
		float getMPH();

		void update();

		bool dataChange();
		bool fixStatus();

		int checksum(const char *sentence);
		bool validateChecksum(std::string sentence);


	private:

		SerialLine serialIn;

		std::string latitude;
		std::string longitude;

		char latDir;
		char longDir;

		int latDeg;
		int longDeg;

		float latDec;
		float longDec;

		time_t lastTime;
		std::string lastTimeStr;

		float speedKnots;
		float speedMPH;

		bool newData;
		bool hasFix;


		void parseLatitude(std::string angle, char dir);
		void parseLongitude(std::string angle, char dir);
		void parseTime(std::string input);
		void parseSpeed(std::string input);

};

#endif
