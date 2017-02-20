/*
 * core.h
 *
 * Created: 8/18/2015 4:35:49 PM
 *  Author: milan_romic
 */ 
#include "xmega/avr_compiler.h"
#include "xmega/usart_driver.h"

#ifndef CORE_H_
#define CORE_H_

#define false	0
#define true	1

#define uniq(LOW,HEIGHT)	(HEIGHT << 8)|LOW		// 2x 8Bit 	--> 16Bit
#define LOW_BYTE(x)        	(x & 0xff)				// 16Bit 	--> 8Bit
#define HIGH_BYTE(x)       	(x >> 8) & 0xff			// 16Bit 	--> 8Bit


#define sbit(ADDRESS,BIT) 	ADDRESS |= (1<<BIT)		// set Bit
#define cbit(ADDRESS,BIT) 	ADDRESS &= ~(1<<BIT)	// clear Bit
#define	toggle(ADDRESS,BIT)	ADDRESS ^= (1<<BIT)		// Bit toggle

#define	bis(ADDRESS,BIT)	ADDRESS & (1<<BIT)		// bit is set?
#define	bic(ADDRESS,BIT)	!(ADDRESS & (1<<BIT))	// bit is clear?

void InitMCU();
void send_str(char* str);

#endif /* CORE_H_ */