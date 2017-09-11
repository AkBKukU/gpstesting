#ifndef RPIGPIO_h
#define RPIGPIO_h

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

class RPIGPIO
{
public:
	RPIGPIO();
	RPIGPIO(int pin);
	~RPIGPIO();

	int setMode(std::string newMode);
	
	int setPin(int pin);
	
	static std::string MODE_INPUT; // "in"
	static std::string MODE_OUTPUT; // "out"
	
	int set(int value);
	static	void setBlock(std::vector<RPIGPIO*> pins, int value);
	int high();
	int low();

	int read();
	

private:
	int exportPin();
	int unexportPin();

	int writeFile(std::string filepath, std::string value);
	std::string readFile(std::string filepath);
	
	bool active;	
	int pinNumber;
	std::string pinString;

	static std::string GPIO_PATH; // "/sys/class/gpio/"
	static std::string GPIO_EXPORT; // "export"
	static std::string GPIO_UNEXPORT; // "unexport"
	static std::string GPIO_VALUE; // "/value"
	static std::string GPIO_DIRECTION; // "/direction"
	static std::string GPIO_DIR; // "gpio"

	std::string gpioExport;
	std::string gpioUnexport;
	std::string gpioValue;
	std::string gpioDirection;
};

#endif
