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
#include "uart.h"
#include "led.h"
#include "MKL25Z4.h"


/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootClocks();

    //Inialize the GPIO for LED blinking
    LED_init();

    //Initialize UART0
    //UART_init();

    //just letting this print to the console window to let me know the code started.
    printf("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++;
        LED_toggle();
    }
    return 0 ;
}

