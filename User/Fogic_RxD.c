/**
  ******************************************************************************
  * @file    Fogic_RxD.c
  * @author  Fogic
  * @version V1.0.0
  * @date    2024-10-13
  * @brief   TxD Main
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

#ifndef __FOGIC_RXD_C__
#define __FOGIC_RXD_C__

// ============================================================================ //
#include "include.h"
#if( FOGIC_PROG_TYP == FOGIC_PROG_RXD )

u8	SysTick_IRQ = 0;
u32 SysTick_CNT = 0;

// ============================================================================ //
u8	RFM_Buf[ FOGIC_PAYLOAD_LEN_MAX ];
u8	URT_Buf[ 24 ];
u8	RxD_Led = 0;
u8	URT_TxN = 0;

u32 PkgIndx = 0x1234ABCD;

// ============================================================================ //
static void Fogic_GPIO_Init( void )
{
	// P05, TxLed / RxLed
	P0_MD0 &= ~0x0C;  	// P01
    P0_MD0 |= 0x04;
	
    P0_MD1 &= ~0x3c;  // P05,P06
    P0_MD1 |= 0x14;
	
    P06 = FOGIC_LED_OFF;	// Close TxLED
	P05 = FOGIC_LED_OFF;	// Close TxLED
	P01 = FOGIC_LED_OFF;	// Close TxLED
}

static void Fogic_RFM_Init( void )
{
	PkgIndx = 0x1234ABCD;
	SysTick_CNT = 0;
	RxD_Led = 0;

	rf_init();
    rf_spi_write_byte( REG_BANK0_FEATURE, 0x10 );  
    rf_spi_write_byte( REG_BANK0_EN_AA, 0x00 );
    rf_spi_write_byte( REG_BANK0_CONFIG, 0xfb );
    rf_spi_write_byte( REG_BANK0_RX_PW_P0, FOGIC_RF_PWP0 );		// 0x20 --> 0x10
    rf_spi_write_byte( REG_BANK0_RF_CH, FOGIC_RF_CHANNEL );
    rf_spi_write_byte( REG_BANK0_EN_RXADDR, 0x03 );  
    rf_flush_tx();
    rf_flush_rx();
    rf_clear_all_irq();     
    rf_ce_high(); 
}

static void Fogic_SysPara_Init( void )
{
	URT_Buf[ 0 ] = 0x5A;
	URT_Buf[ 1 ] = 0xA5;
	URT_Buf[ 2 ] = 0xBF;		// Cmd
	URT_Buf[ 3 ] = 0x01;		// Write
	URT_Buf[ 4 ] = 0x00;		// Rev
	URT_Buf[ 5 ] = 14;			// Cnt
		
	URT_Buf[ 6 ] = 0x00;			// Index
	URT_Buf[ 7 ] = 0x00;			// KeyVal
	//URT_Buf[ 8 ] ~ URT_Buf[ 19 ]	// UUID 12-Byte
	
	URT_Buf[ 20 ] = 0x00;		// CRC
}

void Fogic_Target_Init( void )
{
	Fogic_SysPara_Init();
	Fogic_Uart_Init();
	Fogic_RFM_Init();	
	Fogic_GPIO_Init();
}

// ============================================================================ //
void Fogic_RFM_Reads( void )
{
	u8	RF_DatCnt;
	u8 	RF_Status = rf_spi_read_byte( REG_BANK0_STATUS );
	
    if( RF_Status & RF_STATUS_RX_DR )
	{
		RF_DatCnt = rf_receive_pack( RFM_Buf );
		
		rf_ce_low();
        rf_clear_all_irq();
        rf_flush_rx();
        rf_ce_high();
		
        if( (RF_DatCnt == FOGIC_PAYLOAD_LEN_MAX) && 
			(RFM_Buf[ FOGIC_PAYLOAD_LEN_END ] == Fogic_GetCRC( RFM_Buf, FOGIC_PAYLOAD_LEN_END )) && 
			(PkgIndx != (u32)RFM_Buf[ 1 ])
		  )
		{
			PkgIndx = (u32)RFM_Buf[ 1 ];
			SysTick_CNT = 0;
			
			// Urt Package
			URT_Buf[ 6 ] = RFM_Buf[ 1 ];	// Index
			URT_Buf[ 7 ] = RFM_Buf[ 14 ];	// Active KEY
								
			// UUID
			for( RF_DatCnt = 0; RF_DatCnt < 12; RF_DatCnt++ )
			{
				URT_Buf[ 8 + RF_DatCnt ] = RFM_Buf[ 2 + RF_DatCnt ];
			}
			
			// CRC
			URT_Buf[ 20 ] = Fogic_GetCRC( URT_Buf, 20 );
			URT_TxN = 3;	// Retry 3
			RxD_Led = RFM_Buf[ 14 ];		// Demo LedShow RxCmds
		}
	}
}

static void Fogic_RxLed_Show( void )
{
	switch( RxD_Led )
	{
		default:
		case KEY_NO_FUNC:	
		{
			P01 = FOGIC_LED_OFF;	
			P05 = FOGIC_LED_OFF; 
			P06 = FOGIC_LED_OFF; 
			break;
		}
		case KFN_MOD_INC:
		case 0x11:
		{
			P01 = FOGIC_LED_OFF;	
			P05 = FOGIC_LED_OFF; 
			P06 = FOGIC_LED_ONS; 
			break;
		}
		case KFN_GO_LEFT:
		case 0x12:
		{ 
			P01 = FOGIC_LED_OFF;	
			P05 = FOGIC_LED_ONS; 
			P06 = FOGIC_LED_OFF; 
			break; 
		}
		case KFN_GO_STOP:	
		case 0x13:
		{ 
			P01 = FOGIC_LED_OFF;	
			P05 = FOGIC_LED_ONS; 
			P06 = FOGIC_LED_ONS; 
			break; 
		}
		case KFN_GO_RIGH:
		case 0x14:
		{	
			P01 = FOGIC_LED_ONS;	
			P05 = FOGIC_LED_OFF; 
			P06 = FOGIC_LED_OFF; 
			break; 
		}
		case KFN_MOD_DEC:
		case 0x15:
		{	
			P01 = FOGIC_LED_ONS;	
			P05 = FOGIC_LED_OFF; 
			P06 = FOGIC_LED_ONS; 
			break; 
		}
		case KFN_PAIRNOW:
		case 0x16:
		{	
			P01 = FOGIC_LED_ONS;	
			P05 = FOGIC_LED_ONS; 
			P06 = FOGIC_LED_OFF; 
			break; 
		}
	}
}

// ============================================================================ //
// TMR0 ×ö 1mS SysTick
void Fogic_SysTick_Init( void )
{
    // Config 1ms 
    TMR0_PRL  = 249;        // Period (32M / 128) --> x 250 = 1000
    TMR0_CNTL = 0x00;       // Rec Count Value
    TMR0_CONH = 0xA0;       // Enable TMR0 IRQ
    TMR0_CONL = (((0x7&0x7) << 5)|((0x7&0x7) << 2)|((0x1&0x3) << 0));   // 128DIV, SysClock, Count Mode
	
	__EnableIRQ( TMR0_IRQn );
	IE_EA = 1;
}

// SysTick IRQ
void TMR0_IRQHandler(void) interrupt TMR0_IRQn
{
    if( TMR0_CONH & 0x80 ) 
	{
        TMR0_CONH |= 0x80;
		SysTick_IRQ = 1;
    }
}

// ============================================================================ //
void main( void )
{	
    system_init();
    rf_spi_init();
	Fogic_SysTick_Init();	
	Fogic_Target_Init();
	
    while( 1 )
	{
		if( SysTick_IRQ )
		{
			SysTick_IRQ = 0;
			
			SysTick_CNT++;
			if( SysTick_CNT > 1000 )	// 1 Second
			{
				Fogic_RFM_Init();
			}
			
			Fogic_RFM_Reads();
			if( URT_TxN && ((SysTick_CNT % 50) == 0) )
			{
				URT_TxN--;
				Fogic_Uart_TxString( URT_Buf, 21 ); 
				
				//if( URT_TxN == 0 ) RxD_Led = 0;
				//Fogic_RxLed_Show();
			}
			
			Fogic_RxLed_Show();
		}
	}
}

// ============================================================================ //
#endif
#endif