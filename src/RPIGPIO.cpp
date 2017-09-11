#include "RPIGPIO.h"

/*
 * This is a mulitplatform compatible implimention of GPIO access using SYSFS. 
 *
 * TODO - Rewrite this to use the MEMMAP adress for the I/O block. 
 * Pros:
 *  - Massive performace improvement
 *  - More standard from an embedded ARM programmer stance
 *
 * Cons: 
 *  - Hardware dependant
 *  - Difficult to introduce to beginers
 *
 *  Were this to be done it would need to either rely on compiler definitions 
 *  to tell which device it is(the address is different between Pis) or see if 
 *  there is a way to scan memory to check which Pi it is running on.
 */ 


// Paths and path parts for SYSFS GPIO access
std::string RPIGPIO::GPIO_PATH =      "/sys/class/gpio/";
std::string RPIGPIO::GPIO_EXPORT =    "/sys/class/gpio/export";
std::string RPIGPIO::GPIO_UNEXPORT =  "/sys/class/gpio/unexport";
std::string RPIGPIO::GPIO_VALUE =     "/value";
std::string RPIGPIO::GPIO_DIRECTION = "/direction";
std::string RPIGPIO::GPIO_DIR =       "gpio";

// String to write to "direction" file for GPIO
std::string RPIGPIO::MODE_INPUT = "in";
std::string RPIGPIO::MODE_OUTPUT = "out";


RPIGPIO::RPIGPIO(int pin = 4)
{
	// active is used just to check for first run
	this->active = false;
	this->setPin(pin);
}

/*
 * Destructor
 *
 * Disable the pin as GPIO
 *
 * TODO - Not sure if this is actually working
 */
RPIGPIO::~RPIGPIO()
{
	this->unexportPin();
}

/*
 * Set the direction that the pin communicates data. Either as an input or 
 * output.
 */
int RPIGPIO::setMode(std::string newMode)
{
	int output = this->writeFile(this->gpioDirection,newMode);
	return output;
}

/*
 * Use the given pin number for I/O access. 
 *
 * You can call this at anytime to use a different pin.
 * TODO - Test that ^
 */
int RPIGPIO::setPin(int pin)
{
	// Check if pin was already active before unexporting
	// TODO - Is this even needed?
	if(active)
	{
		this->unexportPin();
	}

	// Save pin info
	this->pinNumber = pin;
	this->pinString = std::to_string(pin);
	
	// Enable the pin as GPIO
	this->exportPin();

	// Wait because the SYSFS method sucks
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Build path to GPIO value file
	std::string pathBuild; 
	pathBuild.append(GPIO_PATH);
	pathBuild.append(GPIO_DIR); 
	pathBuild.append(this->pinString);
	pathBuild.append(GPIO_VALUE);
	this->gpioValue = pathBuild;
	pathBuild.erase();
	
	// Build path to GPIO direction file
	pathBuild.append(GPIO_PATH);
	pathBuild.append(GPIO_DIR); 
	pathBuild.append(this->pinString);
	pathBuild.append(GPIO_DIRECTION);
	this->gpioDirection = pathBuild;
	pathBuild.erase();

	// Mark as active
	this->active = true;

	return this->setMode(MODE_OUTPUT);
}

/*
 * Static function to set multiple GPIO at once using a binary value
 */
void RPIGPIO::setBlock(std::vector<RPIGPIO*> pins, int value)
{
	// Get number of GPIO to use as offset 
	int dataPos = pins.size() - 1;
	
	// Go through each pin
	for ( auto &pin : pins )
	{
		// Shift value and mask to get bit for the pin
		int state = (value >> dataPos) & 0b1;
		pin->set(state);
		dataPos--;
	}
}

/*
 * Set output high or low based on 1 or 0
 */
int RPIGPIO::set(int value)
{
	if(value)
		this->high();
	else
		this->low();
}

/*
 * Set pin high
 */
int RPIGPIO::high()
{
	return this->writeFile(this->gpioValue,"1");
}

/*
 * Set pin low
 */
int RPIGPIO::low()
{
	return this->writeFile(this->gpioValue,"0");
}

/*
 * Read pin status
 */
int RPIGPIO::read()
{
	return atoi(readFile(this->gpioValue).c_str());
}

/*
 * Enable the pin for use as a GPIO
 */
int RPIGPIO::exportPin()
{
	return this->writeFile(GPIO_EXPORT,this->pinString);
}

/*
 * Disable the pin for use as a GPIO
 */
int RPIGPIO::unexportPin()
{
	return this->writeFile(GPIO_UNEXPORT,this->pinString);
}

/*
 * Write the vale to the file at the given path
 */
int RPIGPIO::writeFile(std::string filepath, std::string value)
{
	// Open file for writing
	std::ofstream filewrite(filepath);

	// Verify file is open
	if(filewrite < 0)
		return 1;

	// Write the data and close
	filewrite << value;
	filewrite.close();
	
	return 0;
}

/*
 * Read the content of the given file
 */
std::string RPIGPIO::readFile(std::string filepath)
{	
	// Open file for reading
	std::ifstream fileread(filepath);

	// Verify file is open
	if(fileread < 0)
	{
		std::string fail = "2";
		return fail;
	}

	// Set the contents if the file and close
	std::string output;
	fileread >> output;
	fileread.close();
	
	return output;
}


