#ifndef _MT6816_H
#define _MT6816_H

#include <stdbool.h>	
#include <string.h>		
#include <stdlib.h>		
#include <stdio.h>		
#include <spi.h>		
#include <main.h>		

#define MT6816_SPI_CS_H() GPIOD->ODR|=GPIO_PIN_2;
#define MT6816_SPI_CS_L() GPIOD->ODR&=~GPIO_PIN_2;
#define MT6816_SPI_Get_HSPI (hspi1)
#define MT6816_SPI_Mode (0x03)

typedef struct {
	uint16_t sample_data;
	uint16_t angle;
	bool no_mag_flag;
	bool pc_flag;
} MT6816_SPI_Signal_Typedef;

void MT6816_SPI_Signal_Init(void);
void MT6816_SPI_Get_AngleData(void);

typedef struct {
	uint16_t angle_data;
	uint16_t rectify_angle;
	bool rectify_valid;
} MT6816_Typedef;

extern MT6816_Typedef mt6816;
					
float MT6816_Get_AngleData();




#endif 