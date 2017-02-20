/*
 * pendulum_project.c
 *
 * Created: 8/18/2015 4:31:47 PM
 *  Author: milan_romic
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "core.h"
#include "mechanism.h"
#include "sdcard_driver.h"
#include "xmega/usart_driver.h"
#include "port_driver.h"
#include "globals.h"
#include <string.h>

volatile unsigned char
i, j;

int main(void)
{
	// Initialization of MCU
	InitMCU();	// core.h
	
	// Say Hello!
	send_str("Hello pendulums!\r");	//	core.h

	read_params_SD();
	
    while(1)
    {
		move_motors();
		//Lengetes_1(2);
		
		//inga_1_choreography
		Lengetes_1(inga_1_choreography[i+1]);
		if (inga_1_choreography_timer > (inga_1_progress_time + inga_1_choreography[i]))
		{
			inga_1_progress_time += inga_1_choreography[i];
			if ((i+1) == inga_1_choreography_rowCNT-1) //end of choreography
			{
				i = 0;
				inga_1_progress_time = 0;
				inga_1_choreography_timer = 0;
// 				send_str("\r###end of choreography##\r\r");
// 				send_str("restart first phase: ");
// 				usart_putw_dec(&USART_C0_data, inga_1_choreography[i]);
// 				send_str("\r");
// 				send_str("lokessuruseg: ");
// 				usart_putw_dec(&USART_C0_data, inga_1_choreography[i+1]);
// 				send_str("\r\r");
			}
			else
			{
				i += 2;
				lengesszam_1 = 0;
// 				send_str("jump to next phase: ");
// 				usart_putw_dec(&USART_C0_data, inga_1_choreography[i]);
// 				send_str("\r");
// 				send_str("lokessuruseg: ");
// 				usart_putw_dec(&USART_C0_data, inga_1_choreography[i+1]);
// 				send_str("\r\r");
			}
		}
		
		
		//inga_2_choreography
		Lengetes_2(inga_2_choreography[j+1]);
 		if (inga_2_choreography_timer > (inga_2_progress_time + inga_2_choreography[j]))
 		{
 			inga_2_progress_time += inga_2_choreography[j];
 			if ((j+1) == inga_2_choreography_rowCNT-1)
 			{
 				j = 0;
 				inga_2_progress_time = 0;
 				inga_2_choreography_timer = 0;
				send_str("\r###end of choreography##\r\r");
				send_str("restart first phase: ");
				usart_putw_dec(&USART_C0_data, inga_2_choreography[j]);
				send_str("\r");
				send_str("lokessuruseg: ");
				usart_putw_dec(&USART_C0_data, inga_2_choreography[j+1]);
				send_str("\r\r");
 			}
 			else
 			{
 				j += 2;
 				lengesszam_2 = 0;
				send_str("\rjump to next phase: ");
				usart_putw_dec(&USART_C0_data, inga_2_choreography[j]);
				send_str("\r");
				send_str("lokessuruseg: ");
				usart_putw_dec(&USART_C0_data, inga_2_choreography[j+1]);
				send_str("\r\r");
 			}
 		}
	}
}