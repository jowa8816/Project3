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

#define pQ	15
/**
* define the peak level structured data type
*/
typedef struct
{
	int32_t peak;
	int32_t coeff;
	uint16_t num_samps;			//number of samples in the buffer
}peak_t;

int32_t peak_init(peak_t *p, int32_t coeff, uint16_t buf_size);

void peak_run(peak_t *p, int32_t *buf);


#endif /* PEAK_H_ */
