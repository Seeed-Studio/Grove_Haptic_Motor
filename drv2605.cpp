/*
 * drv2605.cpp
 * A library for Grove-Haptic Motor 1.0
 *
 * Copyright (c) 2015 seeed technology inc.
 * Website    : http://www.seeedstudio.com/
 * Author     : WuRuibin & Sujianzhang
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "drv2605.h"

#if DRV2605_SUPPORT_WIRE_I2C
#include <Wire.h>
#endif

#if DRV2605_SUPPORT_SOFT_I2C
#include <SoftI2CMaster.h>
#endif

#include <Arduino.h>

int DRV2605::init(bool bSoftI2C = false, bool bVerbose = true)
{
	this->bSoftI2C = bSoftI2C;
	this->bVerbose = bVerbose;
	
	/* Check desired I2C mode with supported I2C modes and initialize i2C */
#if DRV2605_SUPPORT_WIRE_I2C
	if (!bSoftI2C)
	{
		Wire.begin();
	}
#else
	if (!bSoftI2C) return -1; /* Wire I2C desired, but not supported */
#endif /* DRV2605_SUPPORT_WIRE_I2C */

#if DRV2605_SUPPORT_SOFT_I2C
	if (bSoftI2C)
	{
		if (!i2c_init()) return -1; /* i2c_init failed */
	}
#else
	if (bSoftI2C) return -1;  /* Soft I2C desired, but not supported */
#endif /* DRV2605_SUPPORT_SOFT_I2C */

	if (bVerbose)
		/* Read the DRV2605 Device ID */
	{
		unsigned char ucDeviceID;
		if (drv2605Read(0x00, (char*)&ucDeviceID) == 0)
		{
			Serial.print("DRV2605 ID: 0x");
			Serial.println(ucDeviceID, HEX);
		}
		else
		{
			Serial.println("Reading DRV2605 ID failed!");
		}
	}

	/* Enter standby mode */
	if (drv2605Write(MODE_Reg, 0x40) != 0) return -1;
	delayMicroseconds(250);

	/* Set rate voltage */
	if (drv2605Write(RATED_VOLTAGE_Reg, 0x50) != 0) return -1;

	/* Set overdrive voltage */
	if (drv2605Write(OD_CLAMP_Reg, 0x89) != 0) return -1;
	
	/* Setup feedback control and control registers */
	if (drv2605Write(FB_CON_Reg, 0xB6) != 0) return -1;
	if (drv2605Write(CONTRL1_Reg, 0x13) != 0) return -1;
	if (drv2605Write(CONTRL2_Reg, 0xF5) != 0) return -1;
	if (drv2605Write(CONTRL3_Reg, 0x80) != 0) return -1;
	
	/* Select the LRA Library */
	if (drv2605Write(LIB_SEL_Reg, 0x06) != 0) return -1;

	/* Put the DRV2605 device in active mode */
	if (drv2605Write(MODE_Reg, 0x00) != 0) return -1;
}

/* Try to read 1 byte from the DRV2605 register at 'regAddress'.
 * The read value is stored in *pcValue
 * Returns 0 on success or -1 on failure.
 */
int DRV2605::drv2605Read(unsigned char ucRegAddress, char* pcValue)
{
#if DRV2605_SUPPORT_WIRE_I2C
	if (!bSoftI2C)
	{
		Wire.beginTransmission(DRV2605_ADDRESS);
		if (Wire.write(ucRegAddress) != 1) return -1;
		if (Wire.endTransmission(false) != 0) return -1;

		Wire.requestFrom(DRV2605_ADDRESS, 1);
		
		while(!Wire.available());
		
		*pcValue = Wire.read();
		
		return 0;
	}
#endif /* DRV2605_SUPPORT_WIRE_I2C */

#if DRV2605_SUPPORT_SOFT_I2C
	if (bSoftI2C)
	{
		/* Send the device slave address and a write bit */
		if (!i2c_start((DRV2605_ADDRESS << 1) | I2C_WRITE)) return -1;

		/* Write the register address that we want to write to */
		if (!i2c_write(ucRegAddress)) return -1;

		/* Resend start condition */
		if (!i2c_rep_start((DRV2605_ADDRESS << 1) | I2C_READ)) return -1;

		/* Read data from register and send NAK */
		*pcValue = i2c_read(true);

		/* Send stop condition */
		i2c_stop();

		return 0;
	}
#endif /* DRV2605_SUPPORT_SOFT_I2C */
}

/* Perform a multi byte read (2 bytes) on the DRV2605.
 * First byte (most significant) will be from register at 'ucRegAddress'.
 * Second byte (least significant) will be from register at 'ucRegAddress' + 1.
 */
int DRV2605::drv2605ReadInt(unsigned char ucRegAddress, int* piValue)
{
#if DRV2605_SUPPORT_WIRE_I2C
	if (!bSoftI2C)
	{
		Wire.beginTransmission(DRV2605_ADDRESS);
		if (Wire.write(ucRegAddress) != 1) return -1;
		if (Wire.endTransmission(false) != 0) return -1;

		Wire.requestFrom(DRV2605_ADDRESS, 2);
		
		while(Wire.available() < 2);
		
		*piValue = ((int)Wire.read()) << 8;
		*piValue |= Wire.read();
		
		return 0;
	}
#endif /* DRV2605_SUPPORT_WIRE_I2C */

#if DRV2605_SUPPORT_SOFT_I2C
	if (bSoftI2C)
	{
		/* Send the device slave address and a write bit */
		if (!i2c_start((DRV2605_ADDRESS << 1) | I2C_WRITE)) return -1;

		/* Write the register address that we want to write to */
		if (!i2c_write(ucRegAddress)) return -1;

		/* Resend start condition */
		if (!i2c_rep_start((DRV2605_ADDRESS << 1) | I2C_READ)) return -1;

		/* Read data from register and send ACK */
		*piValue = ((int)i2c_read(false)) << 8;

		/* Read the last byte from register and send NAK */
		*piValue |= i2c_read(true);
		
		/* Send stop condition */
		i2c_stop();

		return 0;
	}
#endif /* DRV2605_SUPPORT_SOFT_I2C */
}

int DRV2605::drv2605Write(unsigned char ucRegAddress, char cValue)
{
#if DRV2605_SUPPORT_WIRE_I2C
	if (!bSoftI2C)
	{
		Wire.beginTransmission(DRV2605_ADDRESS); // start transmission to device 
		if (Wire.write(ucRegAddress) != 1) return -1;
		if (Wire.write(cValue) != 1) return -1;
		if (Wire.endTransmission() != 0) return -1;
		
		return 0;
	}
#endif /* DRV2605_SUPPORT_WIRE_I2C */

#if DRV2605_SUPPORT_SOFT_I2C
	if (bSoftI2C)
	{
		/* Send the device slave address and a write bit */
		if (!i2c_start((DRV2605_ADDRESS << 1) | I2C_WRITE)) return -1;

		/* Write the register address that we want to write to */
		if (!i2c_write(ucRegAddress)) return -1;
		
		/* Write the data byte */
		if (!i2c_write(cValue)) return -1;
		
		/* Send stop condition */
		i2c_stop();
		
		return 0;
	}
#endif /* DRV2605_SUPPORT_SOFT_I2C */
}

int DRV2605::drv2605_AutoCal(void)
{
    unsigned char temp = 0x00;
    unsigned char ACalComp, ACalBEMF, BEMFGain;

	/* Set rate voltage */
	if (drv2605Write(RATED_VOLTAGE_Reg, 0x50) != 0) return -1;

	/* Set overdrive voltage */
	if (drv2605Write(OD_CLAMP_Reg, 0x89) != 0) return -1;
	
	/* Setup feedback control and control registers */
	if (drv2605Write(FB_CON_Reg, 0xB6) != 0) return -1;
	if (drv2605Write(CONTRL1_Reg, 0x93) != 0) return -1;
	if (drv2605Write(CONTRL2_Reg, 0xF5) != 0) return -1;
	if (drv2605Write(CONTRL3_Reg, 0x80) != 0) return -1;
	
	/* Put the DRV2605 device in auto calibration mode */
	if (drv2605Write(MODE_Reg, 0x07) != 0) return -1;
    
    if (drv2605Write(CONTRL4_Reg, 0x20) != 0) return -1;
	
	/* Begin auto calibration */
    if (drv2605Write(GO_Reg, 0x01) != 0) return -1;
    if (bVerbose) Serial.println("DRV2605 Auto-calibrating...");
	
	/* Poll the GO register until least significant bit is set */
	while ((temp & 0x01) != 0x01)
	{
		if (drv2605Read(GO_Reg, (char*)&temp) != 0) return -1;
	}
	
	/* Read the result of the auto calibration */
	if (drv2605Read(STATUS_Reg, (char*)&temp) != 0) return -1;
	if (bVerbose)
	{
		Serial.print("STATUS_Reg: 0x");
		Serial.println(temp, HEX);
	}
	
	/* Check auto calibration result for success or failure */
    if ((temp & 0x08) != 0x00)
	{
        if (bVerbose) Serial.println("DRV2605 Auto-calibration fail");
		return -1;
	}
	if (bVerbose) Serial.println("DRV2605 Auto-calibration complete");
    
	/* Read the compensation result of the auto calibration */
	if (drv2605Read(A_CAL_COMP_Reg, (char*)&ACalComp) != 0) return -1;
	if (bVerbose)
	{
		Serial.print("Auto-Calibration Compensation Result: 0x");
		Serial.println(ACalComp, HEX);
	}

	/* Read the Back-EMF result of the auto calibration */
	if (drv2605Read(A_CAL_BEMF_Reg, (char*)&ACalBEMF) != 0) return -1;
	if (bVerbose)
	{
		Serial.print("Auto-Calibration Back-EMF Result: 0x");
		Serial.println(ACalBEMF, HEX);
	}

	/* Read the feedback control */
	if (drv2605Read(FB_CON_Reg, (char*)&BEMFGain) != 0) return -1;
	if (bVerbose)
	{
		Serial.print("Feedback Control: 0x");
		Serial.println(BEMFGain, HEX);

		Serial.print("Analog gain of the back-EMF amplifier:");
		if ((BEMFGain & 0x03) == 0) Serial.println("5x");
		else if ((BEMFGain & 0x03) == 1) Serial.println("10x");
		else if ((BEMFGain & 0x03) == 2) Serial.println("20x");
		else Serial.println("30x");
	}
	
	return 0;
}

int DRV2605::drv2605_Play_Waveform(unsigned char ucEffect)
{
	/* Exit standby mode and use internal trigger */
    if (drv2605Write(MODE_Reg, 0x00) != 0) return -1;
    
    if (drv2605Write(WAV_SEQ1_Reg, ucEffect) != 0) return -1;
    if (drv2605Write(WAV_SEQ2_Reg, 0x00) != 0) return -1;

    if (drv2605Write(GO_Reg, 0x01) != 0) return -1;
    
	return 0;
}
