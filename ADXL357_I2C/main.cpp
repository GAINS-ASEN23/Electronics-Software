#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

// ADXL357 includes
#include "adxl357.hpp"

int main(int argc, char** argv)
{
	// Set the I2C bus
	int i2cbus = 2;

	// Set the g range
	int g_range = 10;

	// Set the scale factor [ micro-g / LSB ]
	int scale_factor = 19.5;

	// Initialize the ADXL357
	ADXL357 adxl357(i2cbus, ADXL357_I2C_ADDR, g_range, scale_factor);

	// Loop to pull data, press CTRL-C to end
	while(true)
	{
		// Process the sensor data
		adxl357.process();

		// Print the sensor data
		adxl357.print_vars();
	}

	// Return program
	return 0;
}

