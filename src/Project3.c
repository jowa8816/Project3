/*****************************************************************************
* Copyright (C) 2019 by Jon Warriner
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Jon Warriner and the University of Colorado are not liable for
* any misuse of this material.
*
*****************************************************************************/
/**
* @file Project3.c
* @brief main file for Project 3.
*
* This source file provides the main() function for Project3
*
* @author Jon Warriner
* @date April 16, 2019
* @version 1.0
*
*/
 
#include <stdio.h>
#include "clock_config.h"
#include "adc.h"
#include "uart.h"
#include "dma.h"
#include "led.h"
#include "MKL25Z4.h"

#define TX_BUF_SIZE	32

uint16_t result = 0;

char buf[8] = {0};

/*
 * @brief   Application entry point.
 */
int main(void) {
	int8_t j = 0;

  	/* Init board hardware. */
    BOARD_InitBootClocks();

    //Inialize the GPIO for LED blinking
    LED_init();

    //Initialize the ADC
    ADC_init();

    //Initialize UART0
    UART_init();

    //just letting this print to the console window to let me know the code started.
    printf("Hello World\n");

#ifdef ADC_DEBUG
    //set ADC0 interrupt priority to 0
#define ADC_PRI	0
    NVIC->IP[_IP_IDX(ADC0_IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(ADC0_IRQn)]  & ~(0xFFUL << _BIT_SHIFT(ADC0_IRQn))) |
       (((ADC_PRI << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(ADC0_IRQn)));

    //enable the ADC IRQ
    NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)ADC0_IRQn) & 0x1FUL));

#endif

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++;
#ifndef ADC_DEBUG
        //if a new sample is available grab it from the result register
        //and toggle the LED
        if(ADC_ck_complete())
        {
        	result = ADC_get_result();
        	LED_toggle();
        }

        //When we get to the null we are finished sending the previous result
        if(buf[j] == '\0')
        {
        	//build the string for the next result to display
        	sprintf(buf, "%d\r\n", result);
        	j = 0;
        }

        //poll for the transmitter to be ready for a new character
        if(UART_TX_rdy())
        {
        	UART_TX(buf[j]);
        	j++;
        }
#endif

    }
    return 0 ;
}

#ifdef ADC_DEBUG
void ADC0_IRQHandler(void)
{
	result = ADC_get_result();
	LED_toggle();
}
#endif

