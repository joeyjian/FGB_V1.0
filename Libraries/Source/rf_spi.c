/**
  ******************************************************************************
  * @file    rf_spi.c
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    26-02-2022
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
  *
  *
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "include.h"

/** @addtogroup Template_Project
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  RF spi initialization function
  * @param  None
  * @retval None
  */
void rf_spi_init(void)
{
    // P03: spi data in and out
    // P02: spi clk
    P0_AF0 &= ~0xf0;
    P0_AF0 |= 0x50;
    // P03: io output
    // P02: io output
    P0_MD0 &= ~0xf0;
    // pull down spi clk
    P0_PD |= (1 << 2);
    P0_MD0 |=  0xa0;
    // P00: io output, be used as software controlled cs_n
    P0_MD0 &= ~0x3;
    P0_MD0 |=  0x1;
    // pull up cs_n
    P0_PU |= (1 << 0);
    // reset spi
    SYS_CON0 &= (~0x4);
    // enable clock of spi controller
    CLK_CON3 |= 0x4;
    // dereset spi
    SYS_CON0 |= 0x4;
    // baud rate
    SPI0_BAUD = SPI_BAUD;  
    // enable spi
    SPI0_CON |= 0x05;
    
    CSN_HIGH;
}

/**
  * @brief  RF spi write data function
  * @param  wdata : the data to be written
  * @retval None
  */
void rf_spi_write_data(u8 wdata)
{
    while(!(SPI0_STA & 0x01));
    // config tx mode
    SPI0_CON  &= ~(1 << 5);
    SPI0_DATA = wdata;
    while(!(SPI0_STA & 0x01));
}

/**
  * @brief  RF spi read data function
  * @param  None
  * @retval Read one byte of data
  */
u8 rf_spi_read_data(void)
{
    while(!(SPI0_STA & 0x01));
    // config rx mode
    SPI0_CON |= (1 << 5);
    // trigger receive data
    SPI0_DATA = 0x01; //NOP_N;
    while(!(SPI0_STA & (1 << 0)));
    return SPI0_DATA;
}

/**
  * @brief  RF spi write one byte function 
  * @param  addr  : address
  * @param  wdata : the data to be written
  * @retval None
  */
void rf_spi_write_byte(u8 addr, u8 wdata)
{ 
    CSN_LOW;
    rf_spi_write_data(CMD_W_REGISTER | addr);
    rf_spi_write_data(wdata);
    CSN_HIGH;
}

/**
  * @brief  RF spi read one byte function 
  * @param  addr : address
  * @retval Read data
  */
u8 rf_spi_read_byte(u8 addr)
{
    u8 rdata;
    CSN_LOW;
    rf_spi_write_data(CMD_R_REGISTER | addr);
    rdata = rf_spi_read_data();
    CSN_HIGH;
    return rdata;
}

/**
  * @brief  RF spi write buffer function 
  * @param  addr   : address
  * @param  p_data : the pointer to write data  
  * @param  len    : data length
  * @retval None
  */ 
void rf_spi_write_buf(u8 addr, u8 *p_data, u8 len)
{
    u8 i;
    CSN_LOW;
    rf_spi_write_data(CMD_W_REGISTER | addr);
    for(i = 0; i < len; i++) {
        rf_spi_write_data(p_data[i]);
    }
    CSN_HIGH;
}

/**
  * @brief  RF spi read buffer function 
  * @param  addr   : address
  * @param  p_data : the pointer to read data  
  * @param  len    : data length
  * @retval None
  */ 
void rf_spi_read_buf(u8 addr, u8 *p_data, u8 len)
{
    u8 i;
    CSN_LOW;
    rf_spi_write_data(CMD_R_REGISTER | addr);
    
    for(i = 0; i < len; i++) {
        p_data[i] = rf_spi_read_data();
    }
    CSN_HIGH;
}

/**
  * @brief  RF spi write command operate function 
  * @param  cmd : command operation
  * @retval None
  */
void rf_spi_write_cmd(u8 cmd)
{
    CSN_LOW;
    rf_spi_write_data(cmd);
    CSN_HIGH;
}

/**
  * @brief  RF spi write command and data operate function 
  * @param  cmd   : command operation
  * @param  wdata : write data
  * @retval None
  */
void rf_spi_write_cmd_data(u8 cmd, u8 wdata)
{
    CSN_LOW;
    rf_spi_write_data(cmd);
    rf_spi_write_data(wdata);
    CSN_HIGH;
}

