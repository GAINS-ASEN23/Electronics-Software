#ifndef ADXL357_HPP
#define ADXL357_HPP

#define ADXL357_I2C_BUFFER 0x1D

class ADXL357_Accel {

private:
	// The I2C parameters for the accelerometer
	int I2CBus, I2CAddress;
	char dataBuffer[BMA180_I2C_BUFFER];

	// The three axis acceleration variables
	int X_accel;
	int Y_accel;
	int Z_accel;

	// The sensor temperature
	float accel_temp;

	// Function to convert acceleration values recieved to something readable by humans
	int  convertAcceleration(int msb_addr, int lsb_addr);

	// Write data to a register over I2C
	int  i2c_write(char address, char value);

public:
	// Constructor
	ADXL357_Accel(int bus, int address);
	
	// Desctructor
	~ADXL357_Accel();
	
	void init_adxl357();
	int readFullSensorState();
	float getTemperature();

	int getAccelerationX() { return X_accel; }
	int getAccelerationY() { return Y_accel; }
	int getAccelerationZ() { return Z_accel; }

};

#endif
