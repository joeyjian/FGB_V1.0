/**
  ******************************************************************************
  * @file    Fogic_Com.h
  * @author  Fogic
  * @version V1.0.0
  * @date    2024-10-12
  * @brief   Fogic TxD Lib
  ******************************************************************************
  * @attention
  */ 
  
// ============================================================================ //
#ifndef __FOGIC_COM_H__
#define __FOGIC_COM_H__

#ifdef __cplusplus
 extern "C" {
#endif

// ============================================================================ //
u8 Fogic_GetCRC( u8 *Buf, u8 Cnt );
void Fogic_Uart_Init( void );
void Fogic_Uart_TxString( u8 *Dat, u8 Cnt );

// ============================================================================ //
#ifdef __cplusplus
}
#endif

#endif