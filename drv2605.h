/*
 * drv2605.h
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

#ifndef __DRV2605_H__
#define __DRV2605_H__

#include <Arduino.h>

#define DRV2605_SUPPORT_WIRE_I2C 1
#define DRV2605_SUPPORT_SOFT_I2C 0
//#define SDA_PORT PORTD
//#define SDA_PIN 3
//#define SCL_PORT PORTD
//#define SCL_PIN 2

#define STATUS_Reg          0x00
#define MODE_Reg            0x01
#define RTP_INPUT_Reg       0x02
#define LIB_SEL_Reg         0x03
#define WAV_SEQ1_Reg        0x04
#define WAV_SEQ2_Reg        0x05
#define WAV_SEQ3_Reg        0x06
#define WAV_SEQ4_Reg        0x07
#define WAV_SEQ5_Reg        0x08
#define WAV_SEQ6_Reg        0x09
#define WAV_SEQ7_Reg        0x0A
#define WAV_SEQ8_Reg        0x0B
#define GO_Reg              0x0C
#define ODT_OFFSET_Reg      0x0D
#define SPT_Reg             0x0E
#define SNT_Reg             0x0F
#define BRT_Reg             0x10
#define ATV_CON_Reg         0x11
#define ATV_MIN_IN_Reg      0x12
#define ATV_MAX_IN_Reg      0x13
#define ATV_MIN_OUT_Reg     0x14
#define ATV_MAX_OUT_Reg     0x15
#define RATED_VOLTAGE_Reg   0x16
#define OD_CLAMP_Reg        0x17
#define A_CAL_COMP_Reg      0x18
#define A_CAL_BEMF_Reg      0x19
#define FB_CON_Reg          0x1A
#define CONTRL1_Reg         0x1B
#define CONTRL2_Reg         0x1C
#define CONTRL3_Reg         0x1D
#define CONTRL4_Reg         0x1E
#define VBAT_MON_Reg        0x21
#define LRA_RESON_Reg       0x22

#define DRV2605_ADDRESS 0x5A

/*** Register Bits ***/
/*
#define Dev_Reset           (0x80)
#define STANDBY             (0x40)
#define ACTIVE              (0x00)
*/

class DRV2605
{
public: 
	int init(bool bSoftI2C, bool bVerbose);
	int drv2605Read(unsigned char ucRegAddress, char* pcValue);
	int drv2605ReadInt(unsigned char ucRegAddress, int* piValue);
	int drv2605Write(unsigned char ucRegAddress, char cValue);
	int drv2605_AutoCal(void);
	int drv2605_Play_Waveform(unsigned char ucEffect);

private:
	bool bSoftI2C; /* Use software I2C on digital pins or Wire I2C on designated I2C pins */
	bool bVerbose; /* Write diagnostic information to the serial port */
};

#endif /*  __DRV2605_H__ */
