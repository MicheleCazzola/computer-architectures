#ifndef __BUTTON_H_
#define __BUTTON_H_

#include "lpc17xx.h"

// Numeri di pin dei pulsanti alla porta GPIO 2
#define INT0_PIN 10
#define KEY1_PIN 11
#define KEY2_PIN 12

// lib_button.c
void BUTTON_init(void);
void enable_button(int pin, IRQn_Type IRQn);
void disable_button(int pin, IRQn_Type IRQn);
int disabled_button(int pin);

// IRQ_button.c
void INT0_function(void);
void KEY1_function(void);
void KEY2_function(void);
void EINT1_IRQHandler(void);
void EINT2_IRQHandler(void);
void EINT3_IRQHandler(void);


#endif
