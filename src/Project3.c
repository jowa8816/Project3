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
#include "ring.h"
#include "disp.h"
#include "peak.h"
#include "MKL25Z4.h"

//#define PART_2
//#define PART_3
//#define PART_4
#define PART_5

#if defined(PART_2)
#define TX_BUF_SIZE	32

uint16_t result = 0;

char buf[8] = {0};
#elif defined(PART_3) || defined(PART_4) || defined(PART_5)
int32_t sample_buf[256];
#define TX_BUF_SIZE	32

ring_t *tx_buf = 0;

disp_t disp = {0};

uint16_t buf_idx = 0;

#if defined(PART_5)
peak_t peak = {0};
#endif
#endif

/*
 * @brief   Application entry point.
 */
int main(void) {
#if defined(PART_2) && !defined(ADC_DEBUG)
	int8_t j = 0;
#endif

  	/* Init board hardware. */
    BOARD_InitBootClocks();

    //Inialize the GPIO for LED blinking
    LED_init();

    //Initialize the ADC
    ADC_init();

#ifdef PART_3
    //Initialize the DMA
    DMA_init((uint16_t *)&ADC0->R[0], sample_buf, sizeof(sample_buf));	//single-buffering in this part, use the whole buffer
    tx_buf = ring_init(TX_BUF_SIZE);

    //Initialize the display module
    disp_init(&disp, tx_buf, &UART_EN_TX_INT, sample_buf, sizeof(sample_buf)>>2);
#elif defined(PART_4) || defined(PART_5)
    //Initialize the DMA
    DMA_init((uint16_t *)&ADC0->R[0], &sample_buf[buf_idx], sizeof(sample_buf)>>1);	//double-buffering in this part, use half the buffer

#if defined(PART_4)
    tx_buf = ring_init(TX_BUF_SIZE);

    //Initialize the display module
    disp_init(&disp, tx_buf, &UART_EN_TX_INT, sample_buf, sizeof(sample_buf)>>3);
#elif defined(PART_5)
    tx_buf = ring_init(TX_BUF_SIZE);
    //Initialize the display module
    disp_init_2(&disp, tx_buf, &UART_EN_TX_INT);

    peak_init(&peak, (int32_t)(0.85 * (1<<pQ)), sizeof(sample_buf)>>3);
#endif
#endif

    //Initialize UART0
    UART_init();

    //just letting this print to the console window to let me know the code started.
    printf("Hello World\n");

#if defined(PART_2) && defined(ADC_DEBUG)
    //set ADC0 interrupt priority to 0
#define ADC_PRI	0
    NVIC->IP[_IP_IDX(ADC0_IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(ADC0_IRQn)]  & ~(0xFFUL << _BIT_SHIFT(ADC0_IRQn))) |
       (((ADC_PRI << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(ADC0_IRQn)));

    //enable the ADC IRQ
    NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)ADC0_IRQn) & 0x1FUL));
#elif defined(PART_3) || defined(PART_4) || defined(PART_5)
    //set DMA0 interrupt priority to 0
#define DMA_PRI	0
    NVIC->IP[_IP_IDX(DMA0_IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(DMA0_IRQn)]  & ~(0xFFUL << _BIT_SHIFT(DMA0_IRQn))) |
       (((DMA_PRI << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(DMA0_IRQn)));

    //enable the ADC IRQ
    NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)DMA0_IRQn) & 0x1FUL));

    ADC_en_DMA();	//enable DMA requests from the ADC
#endif

    ADC_Start();	//time to start sampling

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++;
#if defined(PART_2) && !defined(ADC_DEBUG)
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
#elif defined(PART_3) || defined(PART_4)
        Display_task(&disp);
#elif defined(PART_5)
        Display_task_2(&disp);
#endif

    }
    return 0 ;
}

#if defined(PART_2) && defined(ADC_DEBUG)
void ADC0_IRQHandler(void)
{
	result = ADC_get_result();
	LED_toggle();
}
#endif

#if defined(PART_3) || defined(PART_4) || defined(PART_5)
void DMA0_IRQHandler()
{
	LED_set();

	//Clear the DONE bit to acknowledge the DMA interrupt
	DMA_Clear_Done();
#if defined(PART_3)
	//Reconfigure source, destination and size to start a new sequence of DMA transfers
	//In this part we are single-buffering so the destination will always be the beginning of the buffer
	//and the size will be the full buffer length.
    DMA_Reconfig((uint16_t *)&ADC0->R[0],sample_buf, sizeof(sample_buf));
    buf_idx = 0;
#elif defined(PART_4) || defined(PART_5)
	//Reconfigure source, destination and size to start a new sequence of DMA transfers
    //In this part we are double-buffering so the destination will either be the beginning or the middle
    //of the buffer and the size will be half the buffer.
    buf_idx = (buf_idx + (sizeof(sample_buf)>>3)) & ((sizeof(sample_buf)>>2)-1);
    DMA_Reconfig((uint16_t *)&ADC0->R[0],&sample_buf[buf_idx], sizeof(sample_buf)>>1);

#if defined(PART_5)
    //loop through the buffer and compare all values against our current peak value
    //replace the current peak value if we find something higher
    peak_run(&peak, &sample_buf[(buf_idx + (sizeof(sample_buf)>>3)) & ((sizeof(sample_buf)>>2)-1)]);
    //decay the current peak value
    peak_decay(&peak);
    //send the current peak value to the display module
    //thi will trigger a display update
    Display_New_Val(&disp, peak.peak);
#endif
#endif

    //check timing
//	LED_toggle();

#if defined(PART_3) || defined(PART_4)
	Display_Trig(&disp, buf_idx);
#endif

	LED_clear();
}
#endif

#if defined(PART_3) || defined(PART_4) || defined(PART_5)
void UART0_DriverIRQHandler(void)
{
char temp;

	//if the UART is ready to transmit, and we still have data in the buffer, then grab the next character and transmit it.
	if(UART_TX_rdy())
	{
		if(entries(tx_buf) != 0)
		{
			extract(tx_buf, &temp);
			UART_TX(temp);
		}
		else
		{
			UART_DIS_TX_INT();
		}
	}
}
#endif
