/**
 * @file adxl357.cpp
 * @brief The main source file for the sample accel project
 * @author Jason Popich - japopich
 * 
 * The main function and program entry is in this file
 * 
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <chrono>

// ADXL357 includes
#include "adxl357.hpp"

FILE* init_bruh()
{
	// Current time in seconds
	uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
	// Convert time to string
	char timestr[100];
    sprintf(timestr, "%d", sec);

	// Compose filename including path to SD card and timestamp to ensure unique
    char fname[100] = "/mnt/extsd/ADXL357_DATA_";
	// char fname[100] = "DATA_";
    char csv[] = ".csv";
	strcat(fname, timestr);
    strcat(fname, csv);

	
	// Open a file with fname, write only
	FILE *foutput = fopen(fname, "w");

	// Print header to file
	fprintf(foutput, "Temp [deg C],X_accel [g],Y_accel [g],Z_accel [g]\n");

	return foutput;
}


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

	// Open a file to save data to
	FILE *foutput = init_bruh();
	std::cout << "Initialized, bruh.........." << std::endl;

	// Loop to pull data, press CTRL-C to end
	while(true)
	{
		// Process the sensor data
		adxl357.process();


		// Print the sensor data
		// adxl357.print_vars();

		// Save the sensor data to a file
		adxl357.add_line(foutput);
	}

	// Close the save file
	fclose(foutput);

	// Return program
	return 0;
}

