#include "NMEAGPS.h"

NMEAGPS::NMEAGPS(char device[]) : serialIn(device)
{
	// Initialize members
	// TODO - Make init() function for this
	this->latitude = "";
	this->longitude = "";
	this->lastTimeStr = "";

	this->latDeg = 0;
	this->longDeg = 0;

	this->latDec = 0;
	this->longDec = 0;
	
	time(&this->lastTime);

	this->speedKnots = 0;
	this->speedMPH = 0;

	this->hasFix = false;
	newData = false;
};

/*
 * Update the GPS data by reading ihe NMEA output from the serial GPS module
 *
 * TODO - Break this out or reduce it. Perhaps modify SerialLine to have a 
 * lineNext() method that can be used to iterate stored lines.
 */
void NMEAGPS::update()
{
	// Update the serial input
	serialIn.update();
	
	// Check if there are new complete lines
	if (serialIn.linesReady())
	{
		// Store new lines
		std::vector<std::string> lines = serialIn.getLines();

		// Go through all lines availible
		for( auto &line : lines )
		{
			// Parse GPRMC lines for relavent data
			// TODO - Parse all lines for all data
			if( line.find("$GPRMC") != std::string::npos)
			{
				// Print NMEA GPRMC line for logging
				// TODO - Can't pipe this output for some reason
				printf("%s", line.c_str());

				// Tokenize the line using ',' as the delimeter
				std::vector<std::string> tokens;
				std::stringstream ss(line);
				std::string token;
				while(std::getline(ss,token,','))
				{
					tokens.push_back(token);
				}
				
				// Test for valid data
				// TODO - This sucks, check the actual fix sinal
				if(tokens[3] != "")
				{	
					// Parse tokens to store most recent data
					this->parseLatitude(tokens[3],tokens[4].c_str()[0]);
					this->parseLongitude(tokens[5],tokens[6].c_str()[0]);

					this->parseSpeed(tokens[7]);
					this->parseTime(tokens[1]);

					// Mark new data for dataChange() 
					this->newData = true;
					
					// Indicate valid data for fixStatus()
					this->hasFix = true;
				}else{	
					// Indicate invalid data for fixStatus()
					this->hasFix = false;
				}
			}

		}
	}
	
}


/*
 * Test for new data
 *
 * This will always set newData to false to prevent the same data from being 
 * accessed twice unintentionally
 */
bool NMEAGPS::dataChange()
{
	// Store newData value in a temp variable
	bool output = false;
	if(this->newData)
		output = true;

	// Clear to false
	this->newData = false;

	return output;
}

/*
 * Parse the latitude that NMEA returns
 *
 * Text format is HHMM.MMMM & Direction in the Hours Minutes Seconds data format
 * NOTE - Seconds are not returned, insteak Minutes is a decimal value
 */
void NMEAGPS::parseLatitude(std::string input, char dir )
{
	// Clear last value
	this->latitude.erase();
	
	// Save values in HMS format
	// TODO - Variable names do NOT represent what they hold
	this->latDir = dir;
	this->latDeg = atoi(input.substr(0,2).c_str());
	this->latDec = atof(input.substr(2).c_str());

	// Convert HMS format to Decimal format
	// TODO - Store the Decimal format as a float as well
	float realDec = (this->latDeg + (this->latDec / 60)) * (dir == 'S'?-1:1);
	this->latitude.append(std::to_string(realDec));
}

/*
 * Parse the longitude that NMEA returns
 *
 * TODO - This is nearly idential to the LAT format. The different is three 
 * digit Hours (HHHMM.MMMM)Try to merge these two functions.
 */
void NMEAGPS::parseLongitude(std::string input, char dir )
{
	this->longitude.erase();
	this->longDir = dir;
	this->longDeg = atoi(input.substr(0,3).c_str());
	this->longDec = atof(input.substr(3).c_str());
	float realDec = (this->longDeg + (this->longDec / 60)) * (dir == 'W'?-1:1);
	this->longitude.append(std::to_string(realDec));
}

/*
 * Parse the speed that NMEA returns
 *
 * The value returned is in Knots
 */
void NMEAGPS::parseSpeed(std::string input)
{
	// Store Knots
	this->speedKnots = atof(input.c_str());

	// Convert Knots to MPH
	// TODO - Save conversion value as const member
	this->speedMPH = atof(input.c_str()) * 1.15077945;
}

/*
 * Parse time return by NMEA
 *
 * Format is ISO8601 in UTC
 *
 * TODO - This relys on using the system time as a default. All data including 
 * the date is available from the GPS, use it.
 */
void NMEAGPS::parseTime(std::string input)
{
	// Get current system time
	time_t rawTime;
	time (&rawTime);
	
	// Convert system values and overwrite with data from GPS
	// TODO - Something funny is happening with the timezone and the "-7" is
	// a hack workaround
	struct tm * newTime = gmtime(&rawTime);
	newTime->tm_hour = atoi(input.substr(0,2).c_str())-7;
	newTime->tm_min = atoi(input.substr(2,2).c_str());
	newTime->tm_sec = atoi(input.substr(4,2).c_str());
	
	// Store final value
	this->lastTime = mktime(newTime);

	// Create a string representaion of the time
	char timeStr[80];
	memset(timeStr,0,80);
	struct tm * timeinfo = localtime(&this->lastTime);
	strftime(timeStr, 80, "%T", timeinfo); // Time
	this->lastTimeStr.erase();
	this->lastTimeStr.append(timeStr);
}

/*
 * Calculate checksum for NMEA sentence
 */
int NMEAGPS::checksum(const char *sentence)
{
	// Pretty much ripped off from the Wikipedia page on NMEA
	int output = 0;
	while(*sentence)
	{
		// TODO - Not very familiar with "^=", look it up
		output ^= *sentence++;
	}

	return output;
}

/*
 * Validate the checksum for a NMEA sentence
 *
 * TODO - Finish implimentation and use this
 */
bool NMEAGPS::validateChecksum(std::string sentence)
{
	// Verify there is a checksum at the end of the sentence 
	if( sentence[sentence.length()-3] != '*' )
	{
		return false;
	}
	
	// Determine if the sentence parameter starts with a '$' as it is not 
	// part of the sentence checksum. 
	int result = 0;
	if( sentence[0] == '$' )
	{
		result = this->checksum(sentence.substr(1,sentence.length()-5).c_str());
	}else{
		result = this->checksum(sentence.substr(sentence.length()-4).c_str());
	}

	// TODO - Add comparison of checksum from input and calulation to return
	return true;
}


// Get functions for members
std::string NMEAGPS::getTimeStr()
{
	return this->lastTimeStr;
}
std::string NMEAGPS::getLatitude()
{
	return this->latitude;
}
std::string NMEAGPS::getLongitude()
{
	return this->longitude;
}
time_t NMEAGPS::getTime()
{
	return this->lastTime;
}
float NMEAGPS::getMPH()
{
	return this->speedMPH;
}
float NMEAGPS::getKnots()
{
	return this->speedKnots;
}
bool NMEAGPS::fixStatus()
{
	return this->hasFix;
}
