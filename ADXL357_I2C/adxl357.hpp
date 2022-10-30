#ifndef _ADXL357_HPP_
#define _ADXL357_HPP_

// Define the Data Buffer Size using the Datasheet (48 buffers because going to 2f)
#define ADXL357_I2C_BUFFER 0x3FF

// Define the Data Struct to hold the data from the ADXL357
struct ADXL357_data {
	// Status Buffer
	uint8_t status[1];

	// Data Buffer
	uint8_t buffer[ADXL357_I2C_BUFFER];
	
	// Data Variables
	double pressure;
	double temperature;
} ADXL357;

#endif