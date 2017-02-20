/*
 * globals.h
 *
 * Created: 4/19/2011 2:16:36 PM
 *  Author: robert kovacs
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "avr_compiler.h"

///////////////////////////Globalne promenljive///////////////////////
extern volatile unsigned char
i, j, periodus_1_elkezdve, periodus_2_elkezdve, inga_1_step, inga_2_step, inga_1_choreography_rowCNT, inga_2_choreography_rowCNT;

extern volatile signed char
; 
extern volatile unsigned int
;

extern volatile signed int
PWM_R, PWM_L, PWM_L_soft, PWM_R_soft, lokessuruseg,
lengesszam_1, inga_1_period, inga_1_pull_time, inga_1_hold_time, inga_1_realize_time, inga_1_pull_force, inga_1_hold_force, inga_1_realize_force,
lengesszam_2, inga_2_period, inga_2_pull_time, inga_2_hold_time, inga_2_realize_time, inga_2_pull_force, inga_2_hold_force, inga_2_realize_force,
inga_1_choreography[100], inga_2_choreography[100];

extern volatile unsigned long
timer_scaler, timer_10ms, inga_1_period_timer, inga_period_time_2, inga_1_choreography_timer, inga_2_choreography_timer, inga_1_progress_time, inga_2_progress_time;

#endif