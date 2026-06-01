/**
  ******************************************************************************
  * @file    rf_operate.c
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
  * @brief  RF initialization function
  * @param  None
  * @retval None
  */
void rf_init(void)
{
    u8 wdata[5];
    
    rf_spi_write_byte(REG_BANK0_FEATURE, 0x20); // soft_reset
    
    rf_ce_low();    
    rf_spi_write_byte(REG_BANK0_CONFIG, 0x8b); // power up
    delay_ms(3); // wait 3 ms
    rf_spi_write_byte(REG_BANK0_PMU_CTL, 0xa8); // HS6220_PWRDWN = 00
    delay_ms(2);
    rf_spi_write_byte(REG_BANK0_FEATURE, 0x10); // VCO_AMP_TX_MUX = b'0, A2 don't need config this bit
    rf_bank_switch(RF_BANK1);
    rf_spi_write_byte(REG_BANK1_TEST_PKDET, 0x24); // pll_vdiv2_sel = 01, A2 don't need config this bit
    rf_bank_switch(RF_BANK0);
    rf_ce_high();
    delay_ms(1);  
    rf_ce_low(); // 一定要注意，校准的时候CE是低的
    
    while((rf_spi_read_byte(REG_BANK0_RF_SETUP) & 0x20) == 0x00);   //wait cal done
    rf_spi_write_byte(REG_BANK0_RF_SETUP, 0x40);  // cal_en = 0
    
    rf_bank_switch(RF_BANK1);   
    
    wdata[2] = 0x75;  // bp_dac =1 bp_rc = 1
    wdata[1] = 0x98;  // bp_vco_amp = 1 bp_vco_ldo=1
    wdata[0] = 0x20;
    rf_spi_write_buf(REG_BANK1_CAL_CTL, wdata, 3);
    
    wdata[2] = 0x05;
    wdata[1] = 0xa0;
    wdata[0] = 0x03;
    rf_spi_write_buf(REG_BANK1_FAGC_CTRL_1, wdata, 3);  // increase fitler agc threshold start (A2 don't need config this bit) 
        
    wdata[1] = 0xb2;
    wdata[0] = 0xcf;  // increase fitler agc threshold 
    rf_spi_write_buf(REG_BANK1_AGC_CTRL, wdata, 2);     // increase fitler agc threshold end (A2 don't need config this bit) 
    
    wdata[2] = 0x11;
    wdata[1] = 0x04;  
    wdata[0] = 0x1f;    
    rf_spi_write_buf(REG_BANK1_RF_IVGEN, wdata, 3);     //xtal_cc = 0x1f(A2 don't need config this bit, customer sample don't need config config)
    
    rf_bank_switch(RF_BANK0);   
    
    wdata[0] 	= 'F';//0x46;
    wdata[1] 	= 'O';//0x0b;
    wdata[2] 	= 'G';//0xaf;
    wdata[3] 	= 'I';//0x43;
    wdata[4] 	= 'C';//0x98;    
    rf_spi_write_buf(REG_BANK0_RX_ADDR_P0, wdata, 5); // set rx address
    wdata[0] 	= 'F';//0x46;
    wdata[1] 	= 'O';//0x0b;
    wdata[2] 	= 'G';//0xaf;
    wdata[3] 	= 'I';//0x43;
    wdata[4] 	= 'C';//0x98;    
    rf_spi_write_buf(REG_BANK0_TX_ADDR, wdata, 5);   //set tx address
    
    rf_clear_all_irq();
    rf_flush_tx();
    rf_flush_rx();
}

/**
  * @brief  RF CE high function
  * @param  None
  * @retval None
  */
void rf_ce_high(void)
{
    rf_spi_write_cmd(CMD_CE_HIGH);
}

/**
  * @brief  RF CE low function
  * @param  None
  * @retval None
  */
void rf_ce_low(void)
{
    rf_spi_write_cmd(CMD_CE_LOW);
}

/**
  * @brief  RF bank switch
  * @param  bank : bank type 
  * @retval None
  */
void rf_bank_switch(TYPE_ENUM_BANK bank)
{
    u8 sta;
    
    sta = rf_spi_read_byte(REG_BANK0_STATUS);
    if(bank != RF_BANK0) 
	{
        if(!(sta & (RF_BANK1 << 7))) 
		{
            rf_spi_write_cmd_data(CMD_ACTIVATE, CMD_ACTIVATE_DATA);
        }
    } 
	else 
	{
        if(sta & (RF_BANK1 << 7)) 
		{
            rf_spi_write_cmd_data(CMD_ACTIVATE, CMD_ACTIVATE_DATA);
        }
    }
}

/**
  * @brief  RF clear all interrupt function
  * @param  None
  * @retval None
  */
void rf_clear_all_irq(void)
{
    rf_spi_write_byte(REG_BANK0_STATUS, 0x70);
}

/**
  * @brief  RF flush tx function
  * @param  None
  * @retval None
  */
void rf_flush_tx(void)
{
    rf_spi_write_cmd(CMD_FLUSH_TX);
}

/**
  * @brief  RF flush rx function
  * @param  None
  * @retval None
  */
void rf_flush_rx(void)
{
    rf_spi_write_cmd(CMD_FLUSH_RX); 
}

/**
  * @brief  RF mode switch function
  * @param  mode : mode type
  * @retval None
  */
void rf_mode_switch(TYPE_ENUM_MODE mode)
{
    u8 reg_val;
    
    reg_val = rf_spi_read_byte(REG_BANK0_CONFIG);
    if(mode != RF_MODE_RX) 
	{
        reg_val &= 0xfe;
    }
	else 
	{
        reg_val |= 0x01;
    }
		
    rf_spi_write_byte(REG_BANK0_CONFIG, reg_val);
    if(mode == RF_MODE_CARRIER) 
	{
        reg_val = rf_spi_read_byte(REG_BANK0_RF_SETUP) | 0x80;
        rf_spi_write_byte(REG_BANK0_RF_SETUP, reg_val);
        /* 注意：最高位为载波使能位，设置了载波模式之后，CE要拉高，载波才会出来；
            如果不要切换频点，功率等，就不要去在操作RF，否则会看不到载波信号的*/
    }
}

/**
  * @brief  RF send data pack function
  * @param  cmd   : command to send packets
  * @param  p_buf : the pointer to write data  
  * @param  len   : data length, from 1 to 32 bytes
  * @retval None
  */
void rf_send_pack(u8 cmd, u8 *p_buf, u8 len)
{
    u8 sta;
    
    sta = rf_spi_read_byte(REG_BANK0_STATUS);
    if(!(sta & RF_STATUS_TX_FULL)) 
	{
        rf_spi_write_buf(cmd, p_buf, len);
    }
}

/**
  * @brief  RF receive data pack function
  * @param  p_buf : the pointer to read data  
  * @retval data length
  */
u8 rf_receive_pack(u8 *p_buf)
{
    u8 sta;
    u8 fifo_sta;
    u8 len;
    
    sta = rf_spi_read_byte(REG_BANK0_STATUS);
    if(sta & RF_STATUS_RX_DR) 
	{
        do{
			len = rf_spi_read_byte(CMD_R_RX_PL_WID);
            if(len <= RF_FIFO_MAX_PACK_SIZE) 
			{
                rf_spi_read_buf(CMD_R_RX_PAYLOAD, p_buf, len);
            }
			else 
			{
                rf_flush_rx();
            }
            fifo_sta = rf_spi_read_byte(REG_BANK0_FIFO_STATUS);
        }while(!(fifo_sta & RF_FIFO_STATUS_RX_EMPTY)); // not empty continue read out 
        
        rf_spi_write_byte(REG_BANK0_STATUS, sta);   // clear irq 
        return len;
    }
		
    if(sta & (RF_STATUS_RX_DR | RF_STATUS_TX_DS | RF_STATUS_MAX_RT)) 
	{ 
        rf_spi_write_byte(REG_BANK0_STATUS, sta);   //clear irq 
    }
    return 0;
}

/**
  * @brief  RF set rate function
  * @param  rate : the rate to set
  * @retval None
  */
void rf_set_rate(TYPE_ENUM_RATE rate)
{
    u8 val;
    val = rf_spi_read_byte(REG_BANK0_RF_SETUP);
    switch(rate) {
        case RF_RATE_1M:
            val &= 0xf7;
            rf_spi_write_byte(REG_BANK0_RF_SETUP, val);
            break;
        case RF_RATE_2M:
            val |= 0x08;
            rf_spi_write_byte(REG_BANK0_RF_SETUP, val);
            break;
        default:
            break;
    }
}

/**
  * @brief  RF set channel function
  * @param  chn : the channel to set
  * @retval None
  */
void rf_set_chn(u8 chn)
{
	if(chn < 0x80) 
	{
		rf_spi_write_byte(REG_BANK0_RF_CH, chn);
	}
}


