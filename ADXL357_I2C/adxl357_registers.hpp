/**
 * @file adxl357_registers.h
 * @brief The main register header file for the ADXL357 3-Axis Accelerometer
 * @author Jason Popich - japopich
 * 
 * All registers are declared and defined here for use with I2C and/or any other comms interface
 * 
 */

#ifndef _ADXL357_REGISTERS_HPP_
#define _ADXL357_REGISTERS_HPP_

// Define the Data Buffer Size using the Datasheet (which specifies that it goes to 0x3FF)
#define ADXL357_I2C_BUFFER 0x3FF

//-------------------------------------------------------
//                  ADXL357 Registers
//-------------------------------------------------------

#define ADXL357_I2C_ADDR                0x1D /*! ADXL357 I2C address */

#define ADXL357_DEVID_AD                0x00 /* ANALOG DEVICES ID REGISTER */
#define ADXL357_DEVID_MST               0x01 /* ANALOG DEVICES MEMS ID REGISTER */
#define ADXL357_PARTID                  0x02 /* DEVICE ID REGISTER */
#define ADXL357_REVID                   0x03 /* PRODUCT REVISION ID REGISTER */
#define ADXL357_STATUS                  0x04 /* SENSOR STATUS REGISTER */
#define ADXL357_FIFO_ENTRIES            0x05 /* FIFO ENTRIES REGISTER */

/*  DATA REGISTERS */
/* Reference Datasheet for more information on each for bitmashing... */

/*  TEMPERATURE */
#define ADXL357_TEMP2                   0x06 /* TEMPERATURE DATA REGISTER bits [11:8] */
#define ADXL357_TEMP1                   0x07 /* TEMPERATURE DATA REGISTER bits [7:0] */

/* ACCELERATIONS */
#define ADXL357_XDATA3                  0x08 /* X-DATA REGISTERS bits [19:12] */
#define ADXL357_XDATA2                  0x09 /* X-DATA REGISTERS bits [11:4] */
#define ADXL357_XDATA1                  0x0A /* X-DATA REGISTERS bits [3:0] */

#define ADXL357_YDATA3                  0x0B /* Y-DATA REGISTERS bits [19:12] */
#define ADXL357_YDATA2                  0x0C /* Y-DATA REGISTERS bits [11:4] */
#define ADXL357_YDATA1                  0x0D /* Y-DATA REGISTERS bits [3:0] */

#define ADXL357_ZDATA3                  0x0E /* Z-DATA REGISTERS bits [19:12] */
#define ADXL357_ZDATA2                  0x0F /* Z-DATA REGISTERS bits [11:4] */
#define ADXL357_ZDATA1                  0x10 /* Z-DATA REGISTERS bits [3:0] */

/* CONFIGURATION REGISTERS */

#define ADXL357_FIFO_DATA               0x11 /* FIFO ACCESS REGISTER */

#define ADXL357_OFFSET_X_H              0x1E /* X-AXIS TRIM H REGISTER */
#define ADXL357_OFFSET_X_L              0x1F /* X-AXIS TRIM L REGISTER */
#define ADXL357_OFFSET_Y_H              0x20 /* Y-AXIS TRIM H REGISTER */
#define ADXL357_OFFSET_Y_L              0x21 /* Y-AXIS TRIM L REGISTER */
#define ADXL357_OFFSET_Z_H              0x22 /* Z-AXIS TRIM H REGISTER */
#define ADXL357_OFFSET_Z_L              0x23 /* Z-AXIS TRIM L REGISTER */

#define ADXL357_ACT_EN                  0x24 /* ACTIVITY ENABLE REGISTER */
#define ADXL357_ACT_THRESH_H            0x25 /* ACTIVITY THRESHOLD H REGISTER */
#define ADXL357_ACT_THRESH_L            0x26 /* ACTIVITY THRESHOLD L REGISTER */
#define ADXL357_ACT_COUNT               0x27 /* ACTIVITY COUNT THRESHOLD */

#define ADXL357_FILTER                  0x28 /* FILTER SETTINGS REGISTER */

#define ADXL357_FIFO_SAMPLES            0x29 /* FIFO SAMPLES REGISTER */

#define ADXL357_INT_MAP                 0x2A /* INTERRUPT PIN (INTx) FUNCTION MAP REGISTER */
#define ADXL357_SYNC                    0x2B /* DATA SYNCHRONIZATION REGISTER */
#define ADXL357_RANGE                   0x2C /* I2C Speed, Interrupt Polarity, and Accel Range Register */
#define ADXL357_POWER_CTL               0x2D /* POWER CONTROL REGISTER */
#define ADXL357_SELF_TEST               0x2E /* SELF TEST REGISTER */
#define ADXL357_RESET                   0x2F /* RESET REGISTER */

#endif