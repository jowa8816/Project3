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
* @file adc.c
* @brief ADC driver
*
* This source file implements an ADC driver
*
* @author Jon Warriner
* @date April 16, 2019
* @version 1.0
*
*/

#include "MKL25Z4.h"
#include "adc.h"

void ADC_init()
{
    //PORTE = 1 - PORTE clock enabled
    SIM->SCGC5 |= SIM_SCGC5_PORTE(1);

    //Configure ADC pins
    //MUX = 000 - PTE20 configured as ALT0 ADC0_DP0/ADC0_SE0
    PORTE->PCR[20] = PORT_PCR_MUX(0);

    //ADC0 = 1 - ADC0 clock enabled
    //Don't mess with other bits
    SIM->SCGC6 |= SIM_SCGC6_ADC0(1);

	//ADC Configuration Register 1
	//ADLPC = 0 - Normal Power Configuration
	//ADIV = 10 - Clock divide is 4
    //ADLSMP = 1 - Long sample time
	//MODE = 11 - Single-ended 16-bit conversion
	//ADICLK = 1 - ADC clock source is Bus Clock/2
    //The Bus clock is already configured to 24MHz before this.  Here we configure the ADC
    //clock source to bus clock divided by 2 (12MHz) and then further divide by 4 for a
    //3MHz ADC clock.
	ADC0->CFG1 = ADC_CFG1_ADIV(2) | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(1) | ADC_CFG1_ADLSMP(1);

	//ADC Status and Control Register 2
	//ADTRG = 0 - Software Trigger
	//ACFE = 0 - Compare function disabled
	//ACFGT = 0 - doesn't matter, compare disabled
	//ACREN = 0 - range function disabled
	//DMAEN = 0 - DMA disabled
	//REFSEL = 00 - Configure VFEFH and VREFL as reference source
	ADC0->SC2 = 0;

	//ADC Status and Control Register 3
	//CAL = 0 - Disable CAL
	//ADCO = 1 - Enable continuous conversions
	//AVGE = 1 - Enable Averaging
	//AVGS = 01 - 8 samples averaged
	ADC0->SC3 = ADC_SC3_ADCO(1) | ADC_SC3_AVGE(1) | ADC_SC3_AVGS(1);

	// ADC Configuration Register 2
	// MUXSEL = 0 - ADxxa Channels
	// ADACKEN = 0 - Asynchronous Clock Disabled
	// ADHSC = 0 - Normal conversion sequence selected
	// ADLSTS = 00 - Default Longest Sample Time
	ADC0->CFG2 = 0x00000000;

//	// ADC Status and Control Register 1A
//	// Initialize to first channel in sequence
//	//first sample config is at the end of the list
//	//AIEN = 0 - Conversion complete interrupt is disabled
//	//DIFF = 0 - Single-ended conversions and input channels are selected
//	//ADCH = 0000 - DADP0 is selected as input
//#ifdef ADC_DEBUG
//	ADC0->SC1[0] = ADC_SC1_ADCH(0) | ADC_SC1_AIEN(1);	//enable interrupt to check sample rate
//#else
//	//ADC0->SC1[0] = ADC_SC1_ADCH(0);
//#endif

	//AverageNum = 8
	//BCT = 25 ADCK
	//LSTAdder = 20 ADCK
	//HSCAdder = 0 ADCK

	//ConversionTime = AverageNum * (BCT + LSTAdder + HSCAdder) = 360 ADCK
	//360 ADCK * (1/3MHz) = 120us -> 8.33kHz

}

uint8_t ADC_ck_complete()
{
	if(ADC0->SC1[0] & ADC_SC1_COCO_MASK)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

uint16_t ADC_get_result()
{
	return((uint16_t)(ADC0->R[0] & 0x0000FFFF));
}

void ADC_Start()
{
	// ADC Status and Control Register 1A
	// Initialize to first channel in sequence
	//first sample config is at the end of the list
	//AIEN = 0 - Conversion complete interrupt is disabled
	//DIFF = 0 - Single-ended conversions and input channels are selected
	//ADCH = 0000 - DADP0 is selected as input
#ifdef ADC_DEBUG
	ADC0->SC1[0] = ADC_SC1_ADCH(0) | ADC_SC1_AIEN(1);	//enable interrupt to check sample rate
#else
	ADC0->SC1[0] = ADC_SC1_ADCH(0);
#endif
}
void ADC_en_DMA()
{
	//ADC Status and Control Register 2
	//ADTRG = 0 - Software Trigger
	//ACFE = 0 - Compare function disabled
	//ACFGT = 0 - doesn't matter, compare disabled
	//ACREN = 0 - range function disabled
	//DMAEN = 1 - DMA enabled and will assert the ADC DMA request during an ADC conversion complete event
	//REFSEL = 00 - Configure VFEFH and VREFL as reference source
	ADC0->SC2 |= ADC_SC2_DMAEN(1);
}

