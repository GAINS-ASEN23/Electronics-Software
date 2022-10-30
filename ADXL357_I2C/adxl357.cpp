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

#include "adxl357.hpp"
#include "adxl357_registers.hpp"

#define MAX_BUS 64

// This function will offset the address because...
/* !! NOTE: FOR SOME REASON THE REGISTERS ARE OFF BY 1 REGISTER, so 0x01 is actually polling 0x02 on the device... */
uint8_t offset_addr(uint8_t addr)
{
    return addr - 0x01;
}

// I2C Open
// This function opens the I2C device, and returns file
int i2c_open(int I2CBus, uint8_t addr)
{
	// Declare I2C device name char array
	char i2cbuf[MAX_BUS];

	// Assign I2C device bus name
	snprintf(i2cbuf, sizeof(i2cbuf), "/dev/i2c-%d", I2CBus);

	// Declare File Variable
	int file;

	// Open the I2C Device
	if ((file = open(i2cbuf, O_RDWR)) < 0)
	{
		printf("Failed to open I2C BUS %u \n", I2CBus);
		return -1;
	}

	// Open IO operation
	if(ioctl(file, I2C_SLAVE, addr) < 0)
	{
		printf("I2C_SLAVE address %X failed... \n", addr);
		return -1;
	}

	// Return the file if successful
	return file;
}

// I2C Close
// Closes the open I2C file
void i2c_close(int file)
{
	close(file);
}


// I2C Write
// This I2C write function assumes that you are using a one-byte register
// Therefore you must first write the register address and then write
// the value for the register. The ADXL357 is using the one-byte
// registers.
bool i2c_write(int file, uint8_t reg, uint8_t val)
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
bool i2c_read(int file, uint8_t reg, unsigned int byte_count, uint8_t *buffer)
{
	// Make sure the buffer is declared
	if (!ADXL357.buffer)
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
bool INIT_ADXL357(int I2CBus)
{
	// Define the I2C bus that the ADXL357 is on

	// Open I2C for the device address
	int file = i2c_open(I2CBus, ADXL357_I2C_ADDR);
	
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

float get_temperature()
{
    // Get the temperature buffers from the accelerometer
    uint8_t temp2 = ADXL357.buffer[offset_addr(ADXL357_TEMP2)];
    uint8_t temp1 = ADXL357.buffer[offset_addr(ADXL357_TEMP1)];

    // Combine both 8 bit registers to form one 16 bit register
    uint16_t temp_16 = (temp2 << 8) | (temp1 & 0xFF);

    // Divide by 4096 to get as float because 12bit, then apply temperature scale factor per datasheet then offset by 25C offset, and return value
    return (((float)temp_16 / 4096) / -9.05) + 25;
}


float get_accelerations()
{
    // Get the X Data buffers
    uint8_t xdata3 = ADXL357.buffer[offset_addr(ADXL357_XDATA3)];
    uint8_t xdata2 = ADXL357.buffer[offset_addr(ADXL357_XDATA2)];
    uint8_t xdata1 = ADXL357.buffer[offset_addr(ADXL357_XDATA1)];

    uint32_t xdata = (xdata3 << )

    // Get the Y Data buffer
}

int main()
{
	// Set the I2C bus
	int i2cbus = 2;
	
	// Initialize the ADXL357
	INIT_ADXL357(i2cbus);

	// Initialize LOOP
	// Ctrl-C to quit
	while(true)
	{
		// Open the I2C Device
		int file = i2c_open(i2cbus, ADXL357_I2C_ADDR);

		// Check for data in the STATUS register
		i2c_read(file, offset_addr(ADXL357_STATUS), 1, ADXL357.status);
		if (ADXL357.status[0] != 0)
		{
			// Read the Data starting from the first register and it should auto_increment up to 0x3FF per pg. 27
			if(!i2c_read(file, ADXL357_DEVID_AD, ADXL357_I2C_BUFFER, ADXL357.buffer))
			{
				printf("Failed to read data buffers... \n");
				return -1;
			}

            // Print out the temperature
            std::cout << "Temperature: " << get_temperature() << std::endl;
		}

		// Close the I2C Buffer
		i2c_close(file);
	}

	// Return program
	return 0;
}
