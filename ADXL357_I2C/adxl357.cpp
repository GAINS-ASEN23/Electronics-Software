#include "adxl357.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <stdio.h>
#include "BMA180Accelerometer.h"
#include <iostream>
#include <math.h>

#define MAX_BUS 64

ADXL357_Accel::ADXL357_Accel(int bus, int address) {
	this->I2CBus = bus;
	this->I2CAddress = address;
	this->readFullSensorState();
}

void ADXL357_Accel::init_adxl357()
{
	// Set the I2C Settings
	// +- 10g with high speed mode and active low interrupt polarity pg. 39
	
	// Set the power control register to measure mode with temp enabled [000...] pg. 39
}

int ADXL357_Accel::readFullSensorState(){

    char namebuf[MAX_BUS];
    snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            cout << "Failed to open ADXL357 Sensor on " << namebuf << " I2C Bus" << endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
            return(2);
    }

    

    //this->accelerationX = convertAcceleration(ACC_X_MSB, ACC_X_LSB);
    //this->accelerationY = convertAcceleration(ACC_Y_MSB, ACC_Y_LSB);
    //this->accelerationZ = convertAcceleration(ACC_Z_MSB, ACC_Z_LSB);
    //this->calculatePitchAndRoll();
    return 0;
}

int ADXL357_Accel::convertAcceleration(int msb_reg_addr, int lsb_reg_addr){
	short temp = dataBuffer[msb_reg_addr];
	temp = (temp<<8) | dataBuffer[lsb_reg_addr];
	temp = temp>>2;
	temp = ~temp + 1;
	return temp;
}

//  Temperature in 2's complement has a resolution of 0.5K/LSB
//  80h is lowest temp - approx -40C and 00000010 is 25C in 2's complement
//  this value is offset at room temperature - 25C and accurate to 0.5K

float BMA180Accelerometer::getTemperature(){

	int offset = -40;  // -40 degrees C
	this->readFullSensorState();
	char temp = dataBuffer[TEMP]; // = -80C 0b10000000  0b00000010; = +25C
	//char temp = this->readI2CDeviceByte(TEMP);
	//this->readFullSensorState();
    //char temp = dataBuffer[TEMP];
	int temperature;
	if(temp&0x80)	{
		temp = ~temp + 0b00000001;
		temperature = 128 - temp;
	}
	else {
		temperature = 128 + temp;
	}
	this->temperature = offset + ((float)temperature*0.5f);
	//cout << "The temperature is " << this->temperature << endl;
	//int temp_off = dataBuffer[0x37]>>1;
	//cout << "Temperature offset raw value is: " << temp_off << endl;
	return this->temperature;
}

int ADXL357_Accel::i2c_write(char address, char value){

    cout << "Starting  I2C state write" << endl;
    char namebuf[MAX_BUS];
    snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            cout << "Failed to open ADXL357 Sensor on " << namebuf << " I2C Bus" << endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
            return(2);
    }

    char buffer[2];
    	buffer[0] = address;
    	buffer[1] = value;
    if ( write(file, buffer, 2) != 2) {
        cout << "Failure to write values to I2C Device address." << endl;
        return(3);
    }
    close(file);
    return 0;
}
