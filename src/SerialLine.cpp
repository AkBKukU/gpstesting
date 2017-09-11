#include "SerialLine.h"


SerialLine::SerialLine(char device[])
{
	
	this->fd = open(device,O_RDWR | O_NOCTTY);
	
	this->storedLines = 0;

	this->initPortSettings();

	tcflush(this->fd, TCIFLUSH);   // Discards old data in the rx buffer
	
}

SerialLine::~SerialLine()
{

	close(this->fd); /* Close the serial port */
}

int SerialLine::linesReady()
{
	return this->storedLines;
}

std::vector<std::string> SerialLine::getLines()
{
	this->storedLines = 0;
	std::vector<std::string> output = this->lineBuffer;
	this->lineBuffer.clear();
	return output;
}
void SerialLine::update()
{
	char tempBuffer[bufferSizeDefault];
	memset(tempBuffer,0,bufferSizeDefault);
	ssize_t length = read(fd, tempBuffer, this->bufferSizeDefault);
	if (length == -1)
	{
		printf("Error reading from serial port\n");
	}
	else if (length == 0)
	{
		//printf("No more data\n");
	}
	else
	{	
		//printf("%s", tempBuffer);
		for ( int i = 0; i < length; i++)
		{
			if(tempBuffer[i] != '\0')
			{
				this->inputBuffer.push_back(tempBuffer[i]);
				if(tempBuffer[i] == '\n')
				{
					this->saveLine();
				}
			}
		}
	}
}

void SerialLine::saveLine()
{	
//	printf("%s", inputBuffer.c_str());
	this->lineBuffer.push_back(this->inputBuffer);
	this->storedLines++;
	this->inputBuffer.erase();
}
void SerialLine::initPortSettings()
{
	tcgetattr(fd, &this->SerialPortSettings);

	cfsetospeed(&this->SerialPortSettings,B9600);
	this->SerialPortSettings.c_cflag |=  PARENB; /*SET   Parity Bit PARENB*/
	this->SerialPortSettings.c_cflag &= ~CSTOPB; //Stop bits = 1 



	this->SerialPortSettings.c_cflag &= ~CSIZE; /* Clears the Mask	   */
	this->SerialPortSettings.c_cflag |=  CS7;   /* Set the data bits = 8 */


	this->SerialPortSettings.c_cflag &= ~CRTSCTS;	   /* No Hardware flow Control						 */
	this->SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines	   */ 
	
	
	this->SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);		  /* Disable XON/XOFF flow control both i/p and o/p */
	this->SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode							*/

	this->SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
	
	/* Setting Time outs */
	this->SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
	this->SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */


	if((tcsetattr(fd,TCSANOW,&this->SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
		printf("\n  ERROR ! in Setting attributes");
	
}

