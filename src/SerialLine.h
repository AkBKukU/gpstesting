#ifndef SerialLine_h
#define SerialLine_h 

#include <stdio.h>
#include <fcntl.h>  /* File Control Definitions		  */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions		 */
#include <errno.h>  /* ERROR Number Definitions		  */
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

class SerialLine
{

	public:
		SerialLine(char device[]);
		~SerialLine();

		int linesReady();

		std::vector<std::string> getLines();

		void update();

	private:
		int bufferSize;
		int bufferPos;
		static const int bufferSizeDefault = 256;
		std::string inputBuffer;

		void saveLine();
		std::vector<std::string> lineBuffer;
		int storedLines;

		int fd;

		struct termios SerialPortSettings;

		void initPortSettings();

};

#endif
