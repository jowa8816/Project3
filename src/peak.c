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
* @file peak.c
* @brief Peak Level Measurement
*
* This source file implements Peak level measurement
*
* @author Jon Warriner
* @date April 27, 2019
* @version 1.0
*
*/

#include "peak.h"

int32_t peak_init(peak_t *p, int32_t coeff, uint16_t buf_size)
{
	//if any of the pointers are not initialized then return an error
	if((p == 0))
	{
		return(-1);
	}

	p->coeff = coeff;
	p->num_samps = buf_size;
	p->peak = 0;
	p->tmp_peak = 0;

	return(0);
}
