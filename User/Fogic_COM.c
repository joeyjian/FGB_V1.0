/**
  ******************************************************************************
  * @file    Fogic_Com.c
  * @author  Fogic
  * @version V1.0.0
  * @date    2024-10-12
  * @brief   Comm Func
  ******************************************************************************
  * @attention
  */ 

#ifndef __FOGIC_COM_C__
#define __FOGIC_COM_C__

// ============================================================================ //
#include "include.h"

// ============================================================================ //
// 参数定义

// ============================================================================ //
u8 Fogic_GetCRC( u8 *Buf, u8 Cnt )
{
	u8 CrcV = 0;
	u8 i;
	for( i = 0; i < Cnt; i++ )
	{
		CrcV ^= Buf[i];
	}
	return( ~CrcV );
}

// ============================================================================ //
void Fogic_Uart_Init( void )
{
    u32 BaudVal = (FOGIC_SYSCLK - FOGIC_URT_BAUD) / FOGIC_URT_BAUD;
    
    // UART0 IO : TX -- P13(AF3)
    P1_MD0 &= ~0xC0;
    P1_MD0 |=  0x80;
    P1_AF0 |=  0xC0;
    
    while(!(UART0_STA & (1 << 4)));
    
    UART0_BAUD1 = (u8)( BaudVal >> 8 );
    UART0_BAUD0 = (u8)( BaudVal >> 0 );
    UART0_CON   = 0x10;
}

void Fogic_Uart_TxString( u8 *Dat, u8 Cnt )
{
	u8 i;
	for( i = 0; i < Cnt; i++ )
	{
		while( !(UART0_STA & (1 << 4)) );
		UART0_DATA = Dat[i];
		while( !(UART0_STA & (1 << 4)) );
	}
}

// ============================================================================ //
#endif