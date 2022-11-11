/**
 * @file adxl357.hpp
 * @brief The main header file for the ADXL357 3-Axis Accelerometer
 * @author Jason Popich - japopich
 * 
 * The main class declaration lives in this file for the ADXL357 class
 * 
 */

#ifndef _ADXL357_HPP_
#define _ADXL357_HPP_

// Includes
#include "adxl357_registers.hpp"

class ADXL357
{
	public:
		/* CLASS FUNCTIONS */

		// Constructor
		ADXL357(int i2cbus_in, int device_addr_in, int range_in, int scale_factor_in);

		// Destructor
		~ADXL357();

		/* FUNCTIONS */

		// Print Variables
		void print_vars();

		// I2C FUNCTIONS
		uint8_t offset_addr(uint8_t addr);

		int i2c_open();
		void i2c_close(int file);
		bool i2c_write(int file, uint8_t reg, uint8_t val);
		bool i2c_read(int file, uint8_t reg, unsigned int byte_count, uint8_t *buffer);

		// ADXL357 Specific Functions
		bool init_adxl357();
		void calculate_temperature();
		void calculate_accelerations();
		void process();

		// Getters and Setters

		double get_temp()		{ return this->temperature; };					// Returns the temperature in [ degC ]
		double get_x_accel() 	{ return this->x_accel; };						// Returns the x acceleration in [ g ]
		double get_y_accel()	{ return this->y_accel; };						// Returns the y acceleration in [ g ]
		double get_z_accel()	{ return this->z_accel; };						// Returns the z acceleration in [ g ]

		// Save to SD card
		void open_file(FILE *foutput); 			// Save file init
		void add_line(FILE *foutput); 			// Print data to file

	private:

		/* I2C VARIABLES */
		int i2cbus {0};							// The I2C Bus that the device is on
		uint8_t device_addr {0};				// The I2C Device address

		/* CONFIGURATION VARIABLES */
		int range {0};
		int scale_factor {0};

		/* DATA BUFFER VARIABLES */

		uint8_t status[1];						// Status Buffer that will hold the status register values
		uint8_t buffer[ADXL357_I2C_BUFFER];		// Data Buffer that holds every register from the ADXL357
	
		/* SENSOR READINGS - Converted */

		double temperature {0};					// Temperature [ degC ]
		
		double x_accel {0};						// X-Axis Acceleration [ g ]
		double y_accel {0};						// Y-Axis Acceleration [ g ]
		double z_accel {0};						// Z-Axis Acceleration [ g ]
		

	protected:
};

#endif