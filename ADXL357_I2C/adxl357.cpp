/**
 * @file adxl357.cpp
 * @brief The main source file for the ADXL357 3-Axis Accelerometer
 * @author Jason Popich - japopich
 * 
 * The main class definition lives in this file for the ADXL357 class
 * 
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <iostream>

#include <time.h>
#include <string.h>
#include <chrono>

#include "adxl357.hpp"
#include "adxl357_registers.hpp"

#define MAX_BUS 64

ADXL357::ADXL357(int i2cbus_in, int device_addr_in, int range_in, int scale_factor_in)
{
    // Set the configuration values
    this->i2cbus = i2cbus_in;
    this->device_addr = device_addr_in;
    this->range = range_in;
    this->scale_factor = scale_factor_in;

    // Initialize the ADXL357
    this->init_adxl357();
}

ADXL357::~ADXL357()
{

}

// This function will offset the address by 0x01 because...
/* !! NOTE: FOR SOME REASON THE REGISTERS ARE OFF BY 1 REGISTER, so 0x01 is actually polling 0x02 on the device... */
uint8_t ADXL357::offset_addr(uint8_t addr)
{
    return addr - 0x01;
}

// I2C Open
// This function opens the I2C device, and returns file
int ADXL357::i2c_open()
{
	// Declare I2C device name char array
	char i2cbuf[MAX_BUS];

	// Assign I2C device bus name
	snprintf(i2cbuf, sizeof(i2cbuf), "/dev/i2c-%d", this->i2cbus);

	// Declare File Variable
	int file;

	// Open the I2C Device
	if ((file = open(i2cbuf, O_RDWR)) < 0)
	{
		printf("Failed to open I2C BUS %u \n", this->i2cbus);
		return -1;
	}

	// Open IO operation
	if(ioctl(file, I2C_SLAVE, this->device_addr) < 0)
	{
		printf("I2C_SLAVE address %X failed... \n", this->device_addr);
		return -1;
	}

	// Return the file if successful
	return file;
}

// I2C Close
// Closes the open I2C file
void ADXL357::i2c_close(int file)
{
	close(file);
}


// I2C Write
// This I2C write function assumes that you are using a one-byte register
// Therefore you must first write the register address and then write
// the value for the register. The ADXL357 is using the one-byte
// registers.
bool ADXL357::i2c_write(int file, uint8_t reg, uint8_t val)
{
	uint8_t write_buf[2] = { reg, val };
	if (write(file, write_buf, 2) != 2)
	{
		// Return error
		printf("Error failed to write to register %X! \n", reg);
		return false;
	}
	// If not, return succeeded
	return true;
}

// I2C Read
// Reads the specified data buffers
/* !! NOTE: FOR SOME REASON THE REGISTERS ARE OFF BY 1 REGISTER, so 0x01 is actually polling 0x02 on the device... */
/* WRITE IS NOT AFFECTED, BUT READING IS... USE OFFSET_ADDR() for reg you are trying to read from */
bool ADXL357::i2c_read(int file, uint8_t reg, unsigned int byte_count, uint8_t *buffer)
{
	// Make sure the buffer is declared
	if (!this->buffer)
	{
		return false;
	}

	// Write to the register we want to read from
	if(!i2c_write(file, reg, 1))
	{
		printf("Failed to write to register %X for reading... \n", reg);
		return false;
	}

	// Read the specific number of bytes
	if(read(file, buffer, byte_count) != byte_count)
	{
		printf("Failed to read from registers... \n");
		return false;
	}

	// Return true if succeeded
	return true;
}

// This function initializes the ADXL357 according to the datasheet
bool ADXL357::init_adxl357()
{
	// Define the I2C bus that the ADXL357 is on

	// Open I2C for the device address
	int file = this->i2c_open();
	
	// Set the ADXL357_RANGE register
    // Settings: 10g - RESERVED - Active Low - High Speed
    // Value: 01 0000 0 1 -> 0x41
	if(!i2c_write(file, ADXL357_RANGE, 0x41))
	{
		printf("Failed to set ADXL357 Range Register... \n");
		return false;
	}

	// Set the ADXL357_POWER_CTL Register
    // Settings: Measurement Mode - Temp On - Default DRDY - RESERVED
    // Value: 0 0 0 00000
	if(!i2c_write(file, ADXL357_POWER_CTL, 0x00))
	{
		printf("Failed to switch output on ADXL357 to Measurement Mode... \n");
		return false;
	}

	// Close the I2C file
	i2c_close(file);

	return true;
}

void ADXL357::calculate_temperature()
{
    // Get the temperature buffers from the accelerometer
    uint8_t temp2 = this->buffer[offset_addr(ADXL357_TEMP2)];
    uint8_t temp1 = this->buffer[offset_addr(ADXL357_TEMP1)];

    // Combine both 8 bit registers to form one 16 bit register
    uint16_t temp_16 = (temp2 << 8) | (temp1 & 0xFF);

    // Divide by 4096 to get as double because 12bit, then apply temperature scale factor per datasheet then offset by 25C offset, and store value
    this->temperature = (((double)temp_16 / 4096) / -9.05) + 25;
}

void ADXL357::calculate_accelerations()
{
    // Get the configuration values
    int local_range = this->range;
    int local_scale = this->scale_factor;

    // Get the X Data buffers
    uint8_t x_data3 = this->buffer[offset_addr(ADXL357_XDATA3)];
    uint8_t x_data2 = this->buffer[offset_addr(ADXL357_XDATA2)];
    uint8_t x_data1 = this->buffer[offset_addr(ADXL357_XDATA1)];

    // Get the Y Data buffers
    uint8_t y_data3 = this->buffer[offset_addr(ADXL357_YDATA3)];
    uint8_t y_data2 = this->buffer[offset_addr(ADXL357_YDATA2)];
    uint8_t y_data1 = this->buffer[offset_addr(ADXL357_YDATA1)];

    // Get the Y Data buffers
    uint8_t z_data3 = this->buffer[offset_addr(ADXL357_ZDATA3)];
    uint8_t z_data2 = this->buffer[offset_addr(ADXL357_ZDATA2)];
    uint8_t z_data1 = this->buffer[offset_addr(ADXL357_ZDATA1)];

    // Move the bits around according to the datasheet
    uint32_t x_data = ((x_data3 << 16) & 0xFF) | ((x_data2 << 8) & 0xFF) | ((x_data1 << 0) & 0xFF);
    uint32_t y_data = ((y_data3 << 16) & 0xFF) | ((y_data2 << 8) & 0xFF) | ((y_data1 << 0) & 0xFF);
    uint32_t z_data = ((z_data3 << 16) & 0xFF) | ((z_data2 << 8) & 0xFF) | ((z_data1 << 0) & 0xFF);

    // Convert and store the accelerations
    this->x_accel = ((double)x_data * (local_range * local_scale)) / 1000000.0;
    this->y_accel = ((double)y_data * (local_range * local_scale)) / 1000000.0;
    this->z_accel = ((double)z_data * (local_range * local_scale)) / 1000000.0;
}

void ADXL357::process()
{
	// Open the I2C Device
	int file = this->i2c_open();

	// Check for data in the STATUS register
	i2c_read(file, offset_addr(ADXL357_STATUS), 1, this->status);
	if (this->status[0] != 0)
	{
		// Read the Data starting from the first register and it should auto_increment up to 0x3FF per pg. 27
		if(!i2c_read(file, ADXL357_DEVID_AD, ADXL357_I2C_BUFFER, this->buffer))
		{
			printf("Failed to read data buffers... \n");
		}

        // Get the temperature
        this->calculate_temperature();

        // Get the accelerations
        this->calculate_accelerations();
	}

	// Close the I2C Buffer
	i2c_close(file);
}

void ADXL357::print_vars()
{
    std::cout << "Temp: " << this->temperature << " degC X_accel: " << this->x_accel << " g Y_ACCEL: " << this->y_accel << " g Z_ACCEL: " << this->z_accel << std::endl;
}

void ADXL357::open_file(FILE *foutput)
{
	// Desynchronize C++ standard streams
	// std::ios_base::sync_with_stdio(false);

	// Stop the flushing of std::cout before std::cin accepts an input
	// cin.tie(NULL);

	// Current time in seconds
	uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
	// Convert time to string
	char timestr[100];
    sprintf(timestr, "%d", sec);

	// Compose filename including path to SD card and timestamp to ensure unique
    char fname[100] = "/mnt/extsd/ADXL357_DATA_";
    char csv[] = ".csv";
	strcat(fname, timestr);
    strcat(fname, csv);

	
	// Open a file with fname, write only
	foutput = fopen(fname, "w");

	// Print header to file
	fprintf(foutput, "Temp [deg C],X_accel [g],Y_accel [g],Z_accel [g]\n");
}

void ADXL357::add_line(FILE *foutput)
{
	fprintf(foutput, "%d,%d,%d,%d\n", this->temperature, this->x_accel, this->y_accel, this->z_accel);
}



