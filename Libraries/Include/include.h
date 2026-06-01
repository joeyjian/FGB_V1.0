/**
  ******************************************************************************
  * @file    include.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    01-05-2021
  * @brief   Main program body header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
  *
  *
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "tx8w7010.h"
#include "system.h"
#include "intrins.h"
#include "debug.h"
#include "rf_spi.h"
#include "rf_operate.h"
	 
// =========================================================================================== //
#define FOGIC_SYSCLK          		(32000000UL)
#define FOGIC_URT_BAUD        		(115200UL)

#define FOGIC_KYACTV_LOGIC			0		// 0 = 按键低电平有效
	 
#define FOGIC_PAYLOAD_LEN_MAX     	16      // tx/rx maximum length of payload
#define FOGIC_PAYLOAD_LEN_END     	(FOGIC_PAYLOAD_LEN_MAX - 1)
#define	FOGIC_RESEND_CNTS_MAX		5		// Tx Total Count

#define FOGIC_TX8W_UUID_ADDR      	NVR2_START_RADDR    // UUID NVR Bank2
#define	FOGIC_RF_CHANNEL			67 // 111 --> 67
#define	FOGIC_RF_PWP0				0x10

typedef enum
{
	KEY_NO_FUNC = 0,	// No Func, Idle
	KFN_MOD_INC = 1,	// Mode +
	KFN_GO_LEFT = 2,	// Turn Left
	KFN_GO_STOP = 3,	// Shake, STOP
	KFN_GO_RIGH = 4,	// Turn Right
	KFN_MOD_DEC = 5,	// Mode -
	KFN_PAIRNOW = 6,	// Remote Pair
} FOGIC_KEYFUNC_E;

// ============================================================================ //
// 程序模式
#define FOGIC_PROG_RXD				0		// 接收模式  -- 头盔
#define FOGIC_PROG_TXD				1		// 发送模式  -- 遥控器
#define	FOGIC_PROG_TYP				FOGIC_PROG_TXD	// FOGIC_PROG_RXD

// ---------------------------------------------------------------------------- //
// 遥控器
#if( FOGIC_PROG_TYP == FOGIC_PROG_TXD )
	#define	FOGIC_AUTOSLEEP_DLYT		1000	// 1S 后，无操作，自动休眠
	#define	FOGIC_REMOTE_COMTEST		0		// 1 = 公用测试遥控器，0 正常遥控器
	
	#include "Fogic_COM.h"
	#include "Fogic_TxD.h"
	
// ---------------------------------------------------------------------------- //
// 头盔
#else
	#define	FOGIC_LED_ONS				1
	#define	FOGIC_LED_OFF				0

	#include "Fogic_COM.h"
	#include "Fogic_RxD.h"
#endif

// =========================================================================================== //


#ifdef __cplusplus
}
#endif

#endif //__INCLUDE_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/
