/**
  ******************************************************************************
  * @file    main.c
  * @author  Fogic
  * @version V1.0.0
  * @date    2024-10-12
  * @brief   Main program body
  ******************************************************************************
  * @attention
  */ 

#ifndef __FOGIC_MAIN_C__
#define __FOGIC_MAIN_C__

// ============================================================================ //
#include "include.h"

// ============================================================================ //
// 参数定义
u8	SysTick_IRQ = 0;
u32 SysTick_CNT = 0;



// ============================================================================ //
void main( void )
{
    system_init();
    rf_spi_init();
	Fogic_Target_Init();
	Fogic_SysTick_Init();
	
    while( 1 )
	{
		if( SysTick_IRQ )
		{
			SysTick_IRQ = 0;
			SysTick_CNT++;
			
			Fogic_Main_Loop( &SysTick_CNT );
		}
	}
}

// ============================================================================ //
#endif