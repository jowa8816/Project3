/*****************************************************************************
* Copyright (C) 2019 by Jon Warriner
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Jon Warrinerk and the University of Colorado are not liable for
* any misuse of this material.
*
*****************************************************************************/
/**
* @file adc.h
* @brief An abstraction for the ADC driver
*
* This header file provides an abstraction of the functions to
* implement the ADC driver
*
* @author Jon Warriner
* @date April 16 2019
* @version 1.0
*
*/

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

//#define ADC_DEBUG

/**
* @brief Initialize the ADC
*
* Initialize the ADC for the application
*
* @return void.
*/
void ADC_init();

/**
* @brief Check for conversion complete
*
* Poll for conversion complete
*
* @return 1 for complete, 0 for not complete.
*/
uint8_t ADC_ck_complete();

/**
* @brief Read ADC conversion result
*
* Read from the ADC result register
*
* @return conversion result
*/
uint16_t ADC_get_result();

/**
* @brief Start the ADC sampling
*
* Write to ADCH to start sampling channel 0.  Depending on
* build config we may also enable interrupts.
*
* @return void
*/
void ADC_Start();

/**
* @brief Enable the ADC to generate DMA requests
*
* Set DMAEN bit in SC2 to allow ADC to generate DMA requests
*
* @return void
*/
void ADC_en_DMA();

#endif /* ADC_H_ */
