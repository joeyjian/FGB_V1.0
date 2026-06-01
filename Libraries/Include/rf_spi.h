/**
  ******************************************************************************
  * @file    rf_spi.h
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
#ifndef __RF_SPI_H__
#define __RF_SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI_BAUD                         ((SYSCLK-2*2000000UL)/(2*2000000UL))        // 62.5kHz(min), 100kHz, 500kHz, 1MHz, 2MHz, 2.5MHz, 4MHz, 5MHz(max)  
#define CSN                              P00
#define CSN_HIGH                         CSN = 1;  
#define CSN_LOW                          CSN = 0;  

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @addtogroup Template_Project
  * @{
  */

/** @addtogroup RF_SPI
  * @{
  */ 

     
/** @defgroup RF_SPI_Exported_Constants
  * @{
  */
/**
  * @}
  */


/** @defgroup RF_SPI_Exported_Functions
  * @{
  */
    
/**
  * @brief  RF spi initialization function
  * @param  None
  * @retval None
  */
void rf_spi_init(void);

/**
  * @brief  RF spi write data function
  * @param  wdata : the data to be written
  * @retval None
  */
void rf_spi_write_data(u8 wdata);

/**
  * @brief  RF spi read data function
  * @param  None
  * @retval Read one byte of data
  */
u8 rf_spi_read_data(void);

/**
  * @brief  RF spi write one byte function 
  * @param  addr  : address
  * @param  wdata : the data to be written
  * @retval None
  */
void rf_spi_write_byte(u8 addr, u8 wdata);

/**
  * @brief  RF spi read one byte function 
  * @param  addr : address
  * @retval Read data
  */
u8 rf_spi_read_byte(u8 addr);

/**
  * @brief  RF spi write buffer function 
  * @param  addr  : address
  * @param  p_buf : the pointer to write data  
  * @param  len   : data length
  * @retval None
  */ 
void rf_spi_write_buf(u8 addr, u8 *p_buf, u8 len);

/**
  * @brief  RF spi read buffer function 
  * @param  addr   : address
  * @param  p_data : the pointer to read data  
  * @param  len    : data length
  * @retval None
  */ 
void rf_spi_read_buf(u8 addr, u8 *p_data, u8 len);

/**
  * @brief  RF spi write command operate function 
  * @param  cmd : command operation
  * @retval None
  */
void rf_spi_write_cmd(u8 cmd);

/**
  * @brief  RF spi write command and data operate function 
  * @param  cmd   : command operation
  * @param  wdata : write data
  * @retval None
  */
void rf_spi_write_cmd_data(u8 cmd, u8 wdata);


/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif //__RF_SPI_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/

