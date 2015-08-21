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
#include <Wire.h>
#include <Arduino.h>

void DRV2605::init(void)
{
    Wire.begin();
    drv2605ReadID();

    drv2605Write(MODE_Reg, 0x40);    //enter standby mode
    delayMicroseconds(250);
    
    drv2605Write(RATED_VOLTAGE_Reg, 0x50);      //Set Rate Voltage
    drv2605Write(OD_CLAMP_Reg, 0x89);       //Set overdrive Voltage
    drv2605Write(FB_CON_Reg, 0xB6);
    drv2605Write(CONTRL1_Reg, 0x13);
    drv2605Write(CONTRL2_Reg, 0xF5);
    drv2605Write(CONTRL3_Reg, 0x80);
    drv2605Write(LIB_SEL_Reg, 0x06);        //select the LRA Lib
    
    drv2605Write(MODE_Reg, 0x00);     //Put DRV2605 device in active mode
}

// Read 1 byte from the BMP085 at 'address'
// Return: the read byte;
char DRV2605::drv2605Read(unsigned char address)
{
    //Wire.begin();
    unsigned char data;
    Wire.beginTransmission(DRV2605_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();

    Wire.requestFrom(DRV2605_ADDRESS, 1);
    while(!Wire.available());
    return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int DRV2605::drv2605ReadInt(unsigned char address)
{
    unsigned char msb, lsb;
    Wire.beginTransmission(DRV2605_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(DRV2605_ADDRESS, 2);
    while(Wire.available()<2);
    msb = Wire.read();
    lsb = Wire.read();
    return (int) msb<<8 | lsb;
}

void DRV2605::drv2605Write(unsigned char address, byte val)
{
    char i = 1;
    Wire.beginTransmission(DRV2605_ADDRESS); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    i = Wire.endTransmission();     // end transmission
    if(0 != i)
    {
        Serial.print("Transmission error!!!\n");
    }
}

void DRV2605::writeRegister(int deviceAddress, byte address, byte val)
{
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}
int DRV2605::readRegister(int deviceAddress, byte address)
{
    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
    // waiting
    }

    v = Wire.read();
    return v;
}

void DRV2605::drv2605ReadID(void)
{
    unsigned char temp;
    temp = drv2605Read(0);
    Serial.print("DRV2605 ID:0x");
    Serial.println(temp,HEX);
}

void DRV2605::drv2605_AutoCal(void)
{
    unsigned char temp;
    unsigned char ACalComp,ACalBEMF,BEMFGain;

    drv2605Write(RATED_VOLTAGE_Reg, 0x50);      //Set Rate Voltage
    drv2605Write(OD_CLAMP_Reg, 0x89);       //Set overdrive Voltage
    drv2605Write(FB_CON_Reg, 0xB6);
    drv2605Write(CONTRL1_Reg, 0x93);
    drv2605Write(CONTRL2_Reg, 0xF5);
    drv2605Write(CONTRL3_Reg, 0x80);
    
    drv2605Write(MODE_Reg, 0x07);
    
    drv2605Write(CONTRL4_Reg, 0x20);
    drv2605Write(GO_Reg, 0x01);    //begin to auto-cal
    Serial.println("DRV2605 Auto-calibrating");
    while( (drv2605Read(GO_Reg) & 0x01) != 0x01 );
    temp = drv2605Read(STATUS_Reg);
    Serial.print("STATUS_Reg:0x");
    Serial.println(temp,HEX);
    temp &= 0x08;
    if(temp == 0)
        Serial.println("DRV2605 Auto-calibration complete");
    else
        Serial.println("DRV2605 Auto-calibration fail");
    
    ACalComp = drv2605Read(A_CAL_COMP_Reg);
    Serial.print("Auto-Calibration Compensation Result:0x");
    Serial.println(ACalComp,HEX);
    
    ACalBEMF = drv2605Read(A_CAL_BEMF_Reg);
    Serial.print("Auto-Calibration Back-EMF Result:0x");
    Serial.println(ACalBEMF,HEX);
    
    BEMFGain = drv2605Read(FB_CON_Reg);
    Serial.print("Feedback Control:0x");
    Serial.println(BEMFGain,HEX);
    BEMFGain &= 0x03;
    Serial.print("Analog gain of the back-EMF amplifier:");
    if(BEMFGain == 0)
        Serial.println("5x");
    else if(BEMFGain == 1)
        Serial.println("10x");
    else if(BEMFGain == 2)
        Serial.println("20x");
    else 
        Serial.println("30x");
    
}

void DRV2605::drv2605_Play_Waveform(unsigned char effect)
{
    drv2605Write(MODE_Reg, 0x00);//exit standby mode and use internal trigger
    
    drv2605Write(WAV_SEQ1_Reg, effect);
    drv2605Write(WAV_SEQ2_Reg, 0x00);

    drv2605Write(GO_Reg, 0x01);
    
}
