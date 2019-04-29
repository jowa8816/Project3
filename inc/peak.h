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
* @file peak.h
* @brief An abstraction for the peak level measurement functions
*
* This header file provides an abstraction of the functions to
* measure the peak level in a buffer of samples.
*
* @author Jon Warriner
* @date April 27 2019
* @version 1.0
*
*/

#ifndef PEAK_H_
#define PEAK_H_

#include <stdint.h>

#define pQ	16		//16 bits of decimal
/**
* define the peak level structured data type
*/
typedef struct
{
	int32_t peak;
	int32_t tmp_peak;
	int32_t coeff;
	uint16_t num_samps;			//number of samples in the buffer
}peak_t;

/**
* @brief Initialize the peak level module
*
* @param p pointer to a peak structure
* @param coeff coefficient in selected Q format.  Valid range is 0 to 1
* @param buf_size size of buffer we will operate on
*
* @return 1 for success or 0 on failure
*/
int32_t peak_init(peak_t *p, int32_t coeff, uint16_t buf_size);

/**
* @brief Find the peak value in a buffer of data, compare against previously detected peak
*
* @param p pointer to a peak structure
* @param buf pointer to the buffer
*
* @return void
*/
__attribute__((always_inline)) static inline void peak_run(peak_t *p, int32_t *buf)
{
	uint16_t i;

	//if any of the pointers are not initialized just return
	if((p == 0) || (buf == 0))
	{
		return;
	}

	p->tmp_peak = 0;

	for(i = 0; i < p->num_samps; i++)
	{
		if(buf[i] > p->tmp_peak)
		{
			p->tmp_peak = buf[i];
		}
//		else
//		{
//			//coefficient is specified to be between 0 and 1 (in selected Q format)
//			//raw data will range from 0 to 65535 (16-bit ADC)
//			//Q16 is chosen to represent the coefficient.  We can do a 32x32 multiply
//			//without overflowing a 32-bit number since the largest multiply we'd
//			//possibly do is 65536 x 65535 if the coefficient is 1 and the data is
//			//65535.  Since the data and coefficient are both positive we can cast
//			//the multiply to unsigned to avoid sign issues.
//			p->peak = (int32_t)((uint32_t)(p->coeff * p->peak) >> pQ);
//		}
	}
}

/**
* @brief multiply the current peak value by our decay coefficient
*
* @param p pointer to a peak structure
*
* @return void
*/
__attribute__((always_inline)) static inline void peak_decay(peak_t *p)
{
	//if any of the pointers are not initialized just return
	if((p == 0))
	{
		return;
	}

	//if the peak value from the current buffer is greater than the current overall peak then it
	//becomes the new peak.  Otherwise we need to apply a decay to the current peak.
	if(p->tmp_peak > p->peak)
	{
		p->peak = p->tmp_peak;
	}
	else
	{
		//coefficient is specified to be between 0 and 1 (in selected Q format)
		//raw data will range from 0 to 65535 (16-bit ADC)
		//Q16 is chosen to represent the coefficient.  We can do a 32x32 multiply
		//without overflowing a 32-bit number since the largest multiply we'd
		//possibly do is 65536 x 65535 if the coefficient is 1 and the data is
		//65535.  Since the data and coefficient are both positive we can cast
		//the multiply to unsigned to avoid sign issues.
		p->peak = (int32_t)((uint32_t)(p->coeff * p->peak) >> pQ);
	}
}

#endif /* PEAK_H_ */
