/**
  ******************************************************************************
  * @file    rf_operate.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    26-02-2022
  * @brief   Main program body header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
  *
  *
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RF_OPERATE_H__
#define __RF_OPERATE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum {
    RF_BANK0 = 0,       // bank0
    RF_BANK1 = 1,       // bank1
} TYPE_ENUM_BANK;       // BANK num

typedef enum {
    RF_MODE_RX = 0,     // rx mode
    RF_MODE_TX,         // tx mode
    RF_MODE_CARRIER,    // carrier mode
} TYPE_ENUM_MODE;       // rf mode

typedef enum {
    RF_RATE_1M = 0,     // 1M rate
    RF_RATE_2M,         // 2M rate
} TYPE_ENUM_RATE;       // RF rate

/* Private define ------------------------------------------------------------*/
// RF command 
#define CMD_R_REGISTER                  0x00 // Define read command to register
#define CMD_W_REGISTER                  0x20 // Define write command to register
#define CMD_R_RX_PAYLOAD                0x61 // Define RX payload register address
#define CMD_W_TX_PAYLOAD                0xA0 // Define TX payload register address
#define CMD_FLUSH_TX                    0xE1 // Define flush TX register command
#define CMD_FLUSH_RX                    0xE2 // Define flush RX register command
#define CMD_REUSE_TX_PL                 0xE3 // Define reuse TX payload register command
#define CMD_ACTIVATE                    0x50 // Define ACTIVATE features register command
#define CMD_ACTIVATE_DATA               0x53 // Define ACTIVATE DATA register command
#define CMD_R_RX_PL_WID                 0x60 // Define read RX payload width register command
#define CMD_W_ACK_PAYLOAD               0xA8 // Define write ACK payload register command
#define CMD_W_TX_PAYLOAD_NOACK          0xB0 // Define disable TX ACK for one time register command
#define CMD_CE_HIGH                     0xD5 // CE high register command
#define CMD_CE_LOW                      0xD6 // CE low register command
#define CMD_NOP                         0xFF // Define No Operation, might be used to read status register

// Bank0 register address
#define REG_BANK0_CONFIG                0x00  
#define REG_BANK0_EN_AA                 0x01  
#define REG_BANK0_EN_RXADDR             0x02  
#define REG_BANK0_PMU_CTL               0x03  
#define REG_BANK0_SETUP_RETR            0x04  
#define REG_BANK0_RF_CH                 0x05  
#define REG_BANK0_RF_SETUP              0x06  
#define REG_BANK0_STATUS                0x07  
#define REG_BANK0_RX_ADDR_P0            0x0A  
#define REG_BANK0_ACCESS_ADDR           0x0A  
#define REG_BANK0_RX_ADDR_P1            0x0B  
#define REG_BANK0_BLE_TIM_CNT           0x0B  
#define REG_BANK0_RX_ADDR_P2            0x0C  
#define REG_BANK0_BLE_CTL               0x0C  
#define REG_BANK0_TX_ADDR               0x10  
#define REG_BANK0_TX_ADVA_ADDR          0x10  
#define REG_BANK0_RX_PW_P0              0x11  
#define REG_BANK0_TX_HEADER             0x11  
#define REG_BANK0_RX_PW_P1              0x12  
#define REG_BANK0_CRC_INIT              0x12  
#define REG_BANK0_RX_PW_P2              0x13  
#define REG_BANK0_STATUS_EXT            0x16  
#define REG_BANK0_FIFO_STATUS           0x17  
#define REG_BANK0_CONFIG_EXT            0x18  
#define REG_BANK0_DYNPD                 0x1C  
#define REG_BANK0_FEATURE               0x1D  
#define REG_BANK0_SETUP_VALUE           0x1E
#define REG_BANK0_PRE_GURD              0x1F

// Bank1 register address
#define REG_BANK1_CHIP_ID               0x00
#define REG_BANK1_PLL_CTL0              0x01
#define REG_BANK1_PLL_CTL1              0x02
#define REG_BANK1_CAL_CTL               0x03
#define REG_BANK1_STATUS                0x07
#define REG_BANK1_STATE                 0x08
#define REG_BANK1_CHAN                  0x09
#define REG_BANK1_FDEV                  0x0C
#define REG_BANK1_DAC_RANGE             0x0D
#define REG_BANK1_CTUNING               0x0F
#define REG_BANK1_FTUNING               0x10
#define REG_BANK1_RX_CTRL               0x11
#define REG_BANK1_FAGC_CTRL_1           0x13
#define REG_BANK1_DOC_DACI              0x1A
#define REG_BANK1_DOC_DACQ              0x1B
#define REG_BANK1_AGC_CTRL              0x1C
#define REG_BANK1_AGC_GAIN              0x1D
#define REG_BANK1_RF_IVGEN              0x1E
#define REG_BANK1_TEST_PKDET            0x1F

// RF status  
#define RF_STATUS_RX_DR                 0x40
#define RF_STATUS_TX_DS                 0x20
#define RF_STATUS_MAX_RT                0x10
#define RF_STATUS_TX_FULL               0x01

// RF fifo status  
#define RF_FIFO_MAX_PACK_SIZE           0x20
#define RF_FIFO_STATUS_RX_FULL          0x02
#define RF_FIFO_STATUS_RX_EMPTY         0x01

//STATUS_EXT register bit define
#define  TX_AEMPTY                      (1<<0)
#define  RX_AFULL                       (1<<1)
#define  RTC_TIMER_INT                  (1<<2)
#define  RX_TIME_OUT                    (1<<3)
#define  SYNC_DS                        (1<<4)
#define  TX_DS                          (1<<5)
#define  RX_DR                          (1<<6)
#define  EVT_DS                         (1<<7)
#define  RX_SYNC_ERR                    (1<<8)
#define  RX_TYPE_ERR                    (1<<9)
#define  RX_LEN_ERR                     (1<<10)
#define  RX_CRC_ERR                     (1<<11)
#define  RX_NESN_ERR                    (1<<12)
#define  RX_SN_ERR                      (1<<13)
#define  LAST_EMPTY                     (1<<14)
#define  RR                             (1<<15)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @addtogroup Template_Project
  * @{
  */

/** @addtogroup RF_OPERATE
  * @{
  */ 

     
/** @defgroup RF_OPERATE_Exported_Constants
  * @{
  */
/**
  * @}
  */


/** @defgroup RF_OPERATE_Exported_Functions
  * @{
  */
/**
  * @brief  RF initialization function
  * @param  None
  * @retval None
  */
void rf_init(void);

/**
  * @brief  RF CE high function
  * @param  None
  * @retval None
  */
void rf_ce_high(void);

/**
  * @brief  RF CE low function
  * @param  None
  * @retval None
  */
void rf_ce_low(void);

/**
  * @brief  RF bank switch
  * @param  None
  * @retval None
  */
void rf_bank_switch(TYPE_ENUM_BANK bank);

/**
  * @brief  RF clear all interrupt function
  * @param  None
  * @retval None
  */
void rf_clear_all_irq(void);

/**
  * @brief  RF flush tx function
  * @param  None
  * @retval None
  */
void rf_flush_tx(void);

/**
  * @brief  RF flush rx function
  * @param  None
  * @retval None
  */
void rf_flush_rx(void);

/**
  * @brief  RF mode switch function
  * @param  mode : mode type
  * @retval None
  */
void rf_mode_switch(TYPE_ENUM_MODE mode);

/**
  * @brief  RF send data pack function
  * @param  cmd   : command to send packets
  * @param  p_buf : the pointer to write data  
  * @param  len   : data length, from 1 to 32 bytes
  * @retval None
  */
void rf_send_pack(u8 cmd, u8 *p_buf, u8 len);

/**
  * @brief  RF receive data pack function
  * @param  p_buf : the pointer to read data  
  * @retval data length
  */
u8 rf_receive_pack(u8 *p_buf);

/**
  * @brief  RF set rate function
  * @param  rate : the rate to set
  * @retval None
  */
void rf_set_rate(TYPE_ENUM_RATE rate);

/**
  * @brief  RF set channel function
  * @param  chn : the channel to set
  * @retval None
  */
void rf_set_chn(u8 chn);
 

/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif //__RF_OPERATE_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/

