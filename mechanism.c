/*
 * mechanism.c
 *
 * Created: 8/18/2015 4:32:11 PM
 *  Author: milan_romic
 */ 
#include "mechanism.h"
#include "globals.h"
#include "port_driver.h"
#include "TC_driver.h"
#include "sdcard_driver.h"
#include "xmega/usart_driver.h"

#include <ctype.h>
#include <stdio.h>

volatile unsigned char
periodus_1_elkezdve, periodus_2_elkezdve, inga_1_step, inga_2_step, inga_1_choreography_rowCNT, inga_2_choreography_rowCNT;

volatile signed int
PWM_R, PWM_L, PWM_L_soft, PWM_R_soft, lokessuruseg, 
lengesszam_1, inga_1_period, inga_1_pull_time, inga_1_hold_time, inga_1_realize_time, inga_1_pull_force, inga_1_hold_force, inga_1_realize_force,
lengesszam_2, inga_2_period, inga_2_pull_time, inga_2_hold_time, inga_2_realize_time, inga_2_pull_force, inga_2_hold_force, inga_2_realize_force,
inga_1_choreography[100], inga_2_choreography[100];
volatile unsigned long
timer_scaler, timer_10ms, inga_1_period_timer, inga_2_period_timer, inga_1_choreography_timer, inga_2_choreography_timer, inga_1_progress_time, inga_2_progress_time;

ISR(TCE1_OVF_vect) 
{
	timer_scaler++;
	if (timer_scaler>26) //~10ms
	{
		
		timer_scaler = 0;
		timer_10ms++;
		inga_1_period_timer++;
		inga_2_period_timer++;
		
		//soft start
		if (PWM_L < PWM_L_soft)
		PWM_L += 3;
		else if (PWM_L > PWM_L_soft)
		PWM_L -= 3;
		if (PWM_R < PWM_R_soft)
		PWM_R += 3;
		else if (PWM_R > PWM_R_soft)
		PWM_R -= 3;
		
		//1s timer
		if (timer_10ms >= 100)
		{
			inga_1_choreography_timer++;
			inga_2_choreography_timer++;
			timer_10ms = 0;
			PORT_TogglePins(&PORTC, 0x80);
		}
		
	}
}
void reset_controller(void)
{
	PORT_TogglePins(&PORTC, 0xFF);
	delay_us(10000000);
	CCP = 0xD8;                        // Configuration change protection: allow protected IO regiser write
	RST.CTRL = RST_SWRST_bm;           // Request software reset by writing to protected IO register
}
void Lengetes_1(char lokessuruseg)
{
	if (inga_1_period_timer >= inga_1_period)
	{
		inga_1_period_timer = 0;
		periodus_1_elkezdve = 0;
		inga_1_step = 1;
		if (lokessuruseg == 0)
		{
			return;
		}
		lengesszam_1++;
		if (lengesszam_1 >= lokessuruseg)
		{
			lengesszam_1 = 0;
			periodus_1_elkezdve = 1;
		}
	}
	
	if (periodus_1_elkezdve == 1)
	{
		switch (inga_1_step)
		{
			case 1:	//pull
				PWM_R_soft = inga_1_pull_force;
				if (inga_1_period_timer >= inga_1_pull_time)
				{
					inga_1_step = 2;
				}
			break;
			case 2://hold
				PWM_R_soft = inga_1_hold_force;
				if (inga_1_period_timer >= (inga_1_pull_time + inga_1_hold_time))
				{
					inga_1_step = 3;
				}
			break;
			case 3:	// realise
				PWM_R_soft = inga_1_realize_force;
				if (inga_1_period_timer >= (inga_1_pull_time + inga_1_hold_time + inga_1_realize_time))
				{
					inga_1_step = 4;
				}
			break;
			case 4:	//stop and wait until the period ends
				PWM_R_soft = 0;
			break;
		}
	}
}

void Lengetes_2(char lokessuruseg)
{
	if (inga_2_period_timer >= inga_2_period)
	{
		inga_2_period_timer = 0;
		periodus_2_elkezdve = 0;
		inga_2_step = 1;
		if (lokessuruseg == 0)
		{
			return;
		}
		lengesszam_2++;
		if (lengesszam_2 >= lokessuruseg)
		{
			lengesszam_2 = 0;
			periodus_2_elkezdve = 1;
		}
	}
	
	if (periodus_2_elkezdve == 1)
	{
		switch (inga_2_step)
		{
			case 1:	//pull
			PWM_L_soft = inga_2_pull_force;
			if (inga_2_period_timer >= inga_2_pull_time)
			{
				inga_2_step = 2;
			}
			break;
			case 2://hold
			PWM_L_soft = inga_2_hold_force;
			if (inga_2_period_timer >= (inga_2_pull_time + inga_2_hold_time))
			{
				inga_2_step = 3;
			}
			break;
			case 3:	// realise
			PWM_L_soft = inga_2_realize_force;
			if (inga_2_period_timer >= (inga_2_pull_time + inga_2_hold_time + inga_2_realize_time))
			{
				inga_2_step = 4;
			}
			break;
			case 4:	//stop and wait until the period ends
			PWM_L_soft = 0;
			break;
		}
	}
}

void move_motors()
{
	
	//desni motor
	if (PWM_R > 10)	//smer 1
	{
		PORT_ClearPins(&PORTH, 0b00000001);	//IN_A1=0
		PORT_SetPins(&PORTH, 0b00001000);	//IN_B1=1
		TCF1.CCABUF = PWM_R;
	}
	else if (PWM_R < -10)	//smer 2
	{
		PORT_ClearPins(&PORTH, 0b00001000);	//IN_B1=0
		PORT_SetPins(&PORTH, 0b00000001);	//IN_A1=1
		
		TCF1.CCABUF = - PWM_R;
	}
	else	//kocenje
	PORT_ClearPins(&PORTH, 0b00001001);	//IN_A1=0, IN_B1=0
	
	
	//levi motor
	if (PWM_L > 10) //smer 1
	{
		PORT_ClearPins(&PORTH, 0b00010000);	//IN_A2=0
		PORT_SetPins(&PORTH, 0b10000000);	//IN_B2=1
		TCF1.CCBBUF = PWM_L;
	}
	else if (PWM_L < -10)	//smer 2
	{
		
		PORT_ClearPins(&PORTH, 0b10000000);	//IN_B2=0
		PORT_SetPins(&PORTH, 0b00010000);	//IN_A2=1,
		TCF1.CCBBUF = - PWM_L;
	}
	else //kocenje
	PORT_ClearPins(&PORTH, 0b10010000);	//IN_A2=0, IN_B2=0
	
};

/* Reads all lines that contain integers from "filename" into numbers 
   Returns the number of read lines
   Note: No line may be longer than 255 characters.
         All lines that contain a symbol different from a number are ignored.
 */
int read_a_SD_file(const char *filename, int numbers[])
{
	const int buf_len = 255;
	sd_file_open(filename);
	int numberCount = 0;
	char linebuf[buf_len];
	int current;
	int posInLine = 0;
	int ignoreThisLine = 0;
	
	memset(linebuf, 0, buf_len);
	for (current = sd_file_getchar(); current != EOF; current = sd_file_getchar()) {
		delay_us(10000);
		if (isdigit(current) || (current == '-' && posInLine == 0)) {
			linebuf[posInLine++] = (char)current;
		} else if (current == '\r' || current == '\n') {
			if (!ignoreThisLine && posInLine != 0) {
				numbers[numberCount++] = atoi(linebuf);
				//usart_putw_dec(&USART_C0_data, numbers[numberCount-1]);
				//send_str("\r");
			}
			posInLine = 0;
			memset(linebuf, 0, buf_len);
			ignoreThisLine = 0;
		} else {
			// If you have a line like "1. Abschnitt" it will be ignored (and not taken as value 1)
			ignoreThisLine = 1;
			//send_str("ignored\r");
		}
	}
	 // Handle unfinished lines at line end
	 if (!ignoreThisLine && posInLine != 0)
	 numbers[numberCount++] = atoi(linebuf);
	sd_file_close();
	return numberCount;
}

void read_params_SD()
{	
	sd_card_open();	
	delay_us(1000000);

	// INIT fileformat:
	// 	inga_1_period:
	// 	600
	// 	inga_1_pull_time:
	// 	200
	// 	inga_1_pull_force:
	// 	250
	// 	inga_1_hold_time:
	// 	100
	// 	inga_1_hold_force:
	// 	100
	// 	inga_1_realize_time:
	// 	150
	// 	inga_1_realize_force:
	// 	-150	
	
	//CHOREOGRAPHY fileformat:
	//páratlan sor-ido, páros sor-lökéssuruség
	//30
	//1
	//60
	//0
	//60
	//2

 	//char minka[255];	
	int minkaArr[255];
	char filename[30];
	int rowCount;
	
	//elso inga INIT
	rowCount = read_a_SD_file("pendulum_1_init.txt", minkaArr);
	send_str("\rbr.redova: ");
	usart_putw_dec(&USART_C0_data, rowCount);
	send_str("\r########### pendulum_1_init.txt ##########\r");
	for(int i = 0; i < rowCount; i++)
	{
		usart_putw_dec(&USART_C0_data, minkaArr[i]);
		send_str("\r");
	}
	send_str("\r####################################\r");
	inga_1_period = minkaArr[0];
	inga_1_pull_time = minkaArr[1];
	inga_1_pull_force = minkaArr[2];
	inga_1_hold_time = minkaArr[3];
	inga_1_hold_force = minkaArr[4];
	inga_1_realize_time = minkaArr[5];
	inga_1_realize_force = minkaArr[6];
	
	delay_us(1000000);
		
	/////masodik inga INIT
	rowCount = read_a_SD_file("pendulum_2_init.txt", minkaArr);
		send_str("\rbr.redova: ");
		usart_putw_dec(&USART_C0_data, rowCount);
	send_str("\r########### pendulum_2_init.txt ##########\r");
	for(int i = 0; i < rowCount; i++)
	{
		usart_putw_dec(&USART_C0_data, minkaArr[i]);
		send_str("\r");
	}
	send_str("\r#####################################\r");
	inga_2_period = minkaArr[0];
	inga_2_pull_time = minkaArr[1];
	inga_2_pull_force = minkaArr[2];
	inga_2_hold_time = minkaArr[3];
	inga_2_hold_force = minkaArr[4];
	inga_2_realize_time = minkaArr[5];
	inga_2_realize_force = minkaArr[6];
			
	delay_us(1000000);
		
	/////inga_1 choreography
	inga_1_choreography_rowCNT = read_a_SD_file("pendulum_1_choreography.txt", minkaArr);
	send_str("\rbr.redova: ");
	usart_putw_dec(&USART_C0_data, inga_1_choreography_rowCNT);
	send_str("\r########### pendulum_1_choreography.txt ##########\r");
	for (int i = 0; i < inga_1_choreography_rowCNT; i++)
	{
			inga_1_choreography[i] = minkaArr[i];
			usart_putw_dec(&USART_C0_data, inga_1_choreography[i]);
			send_str("\r");
	}
	send_str("\r####################################\r");
	
	////inga_2 choreography
	inga_2_choreography_rowCNT = read_a_SD_file("pendulum_2_choreography.txt", minkaArr);
	send_str("\rbr.redova: ");
	usart_putw_dec(&USART_C0_data, inga_2_choreography_rowCNT);
	send_str("\r########### pendulum_2_choreography.txt ##########\r");
	for (int i = 0; i < inga_2_choreography_rowCNT; i++)
	{
		inga_2_choreography[i] = minkaArr[i];
		usart_putw_dec(&USART_C0_data, inga_2_choreography[i]);
		send_str("\r");
	}
	send_str("\r####################################\r");
	
	sd_card_close();
};