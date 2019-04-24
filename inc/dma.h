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
* @file dma.h
* @brief An abstraction for the DMA driver
*
* This header file provides an abstraction of the functions to
* implement the DMA driver
*
* @author Jon Warriner
* @date April 23 2019
* @version 1.0
*
*/


#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>

/**
* @brief Initialize the DMA
*
* Initialize the DMA for the application
*
* @return void.
*/
void DMA_init();


#endif /* DMA_H_ */
