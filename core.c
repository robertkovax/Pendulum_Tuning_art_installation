/*
 * core.c
 *
 * Created: 8/18/2015 4:35:37 PM
 *  Author: milan_romic
 */ 
#include "core.h"
#include "port_driver.h"
#include "TC_driver.h"

void InitMCU()
{
	//Disable interrupts
	cli();
	
	/* --- CLOCK --- */
	//Enable internal 32MHz oscillator
	OSC.CTRL = OSC_RC32MEN_bm;
	//Wait for internal oscillator to stabilize
	while ( !(OSC.STATUS & OSC_RC32MRDY_bm) );
	//Disable change protection for clock
	CCP = CCP_IOREG_gc;
	//CLK.PSCTRL = CLK_PSBCDIV_2_2_gc;
	//Change clock control register
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	
	
	/* --- PORTS --- */
	sbit(PORTE.DIR,0);	sbit(PORTE.OUT,0);	// Output	SDA
	sbit(PORTE.DIR,1);	sbit(PORTE.OUT,1);	// Output	SCL
	sbit(PORTD.DIR,0);	sbit(PORTD.OUT,0);	// Output	LED 3
	
	/* --- TIMER F0 --- */
	TCF0.CNT		= 0;
	TCF0.CTRLA		= TC_CLKSEL_DIV1024_gc;				// Prescaler 1024
	TCF0.CTRLB		= TC_WGMODE_NORMAL_gc;				// Mode Normal
	TCF0.PER		= 32;								// Top-Value 1000 Hz
	TCF0.INTCTRLA	= PMIC_MEDLVLEN_bm;					// Interrupt level
	
	/* --- Interrupts --- */
	// Enable high level interrupts in the PMIC.
	PMIC.CTRL |= PMIC_HILVLEN_bm;
	// Enable medium level interrupts in the PMIC.
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	// Enable low level interrupts in the PMIC.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	//round-robin algoritam EN
	PMIC.CTRL |= PMIC_RREN_bm;
	
	/* --- USART_C0 --- */
	//PC3 (TXC0) - output
	PORTC.DIR &= PIN3_bm;
	//PC2 (RXC0) - input
	PORTC.DIR  |= ~PIN2_bm;
	//Usage of USARTC0 and buffer initialization
	USART_InterruptDriver_Initialize(&USART_C0_data, &USARTC0, USART_DREINTLVL_LO_gc);
	//USARTC0, 8 Data bits, No Parity, 1 Stop bit.
	USART_Format_Set(USART_C0_data.usart, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	//Activate RXC interrupt-a
	USART_RxdInterruptLevel_Set(USART_C0_data.usart, USART_RXCINTLVL_LO_gc);
	//Config Baud rate
	USART_Baudrate_Set(&USARTC0, 3301, -5 );	//19200
	//Activate RX i TX
	USART_Rx_Enable(USART_C0_data.usart);
	USART_Tx_Enable(USART_C0_data.usart);
	
	
	/* --- PWM -- */
	HIRESF_CTRLA = 0b00000010; //enable hi-res za TCF1
	// Configure timer
	TCF1.PER = 800;			//PWM tajmer broji do 800 - frekvenicja 20KHz
	TCF1.CTRLB = TC1_CCBEN_bm | TC1_CCAEN_bm | TC_WGMODE_DS_T_gc;
	TCF1.CTRLA = TC_CLKSEL_DIV1_gc;
	PORT_SetPinsAsOutput( &PORTF, 0b00110000 ); //PF5 - PWM_L, PF4 - PWM_D
	
	PORT_SetPinsAsOutput(&PORTH, 0xFF); //PH0-IN_A1, PH1-EN_A1, PH2-EN_B1, PH3-IN_B1, PH4-IN_A2, PH5-EN_A2, PH6-EN_B2, PH7-IN_B2
	PORT_ClearPins(&PORTH, 0xFF);
	PORT_SetPins(&PORTH, 0b01100110); // EN ALL
	
	/* --- MAIN TIMER TCE1 -- */
	/* Set period ( TOP value ). */
	TC_SetPeriod( &TCE1, 0x002F ); //0x00BF = 12ms //0x5F = 6ms //0x2F = 3ms
	/* Enable overflow interrupt at low level */
	TC1_SetOverflowIntLevel( &TCE1, TC_OVFINTLVL_MED_gc );
	/* Start Timer/Counter. */
	TC1_ConfigClockSource( &TCE1, TC_CLKSEL_DIV256_gc );

	
	// Enable inerrupts
	sei();
}

ISR(USARTC0_RXC_vect)
{
	USART_RXComplete(&USART_C0_data);
}

ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(&USART_C0_data);
}

void send_str(char* str)
{
	while(*str!='\0')
	{
		USART_TXBuffer_PutByte(&USART_C0_data, *str);		//H
		str++;
	}
}

int blink_interval = 0;
ISR(TCF0_OVF_vect) //Timer interrupt 1 ms
{
	blink_interval--;
	if(blink_interval <= 0)
	{
		toggle(PORTD.OUT,0); // LED blinking
		blink_interval = 200;
	}
}