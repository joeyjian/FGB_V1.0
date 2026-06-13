/**
  ******************************************************************************
  * @file    Fogic_TxD.c
  * @brief   按键切换8种LED模式，每次按键：立即发RF -> 显示5s -> 休眠
  *
  * 按键每按一次，模式依次循环（0->1->...->10->0）：
  *   模式0:  常亮-红
  *   模式1:  常亮-橙
  *   模式2:  常亮-黄
  *   模式3:  常亮-绿
  *   模式4:  常亮-青
  *   模式5:  常亮-蓝
  *   模式6:  常亮-紫
  *   模式7:  常亮-品红
  *   模式8:  白色呼吸     亮度0->满->0，循环
  *   模式9:  彩色呼吸     每种颜色各完整呼吸一次
  *   模式10: 全彩渐变     红->橙->黄->绿->青->蓝->紫->红 平滑过渡
  *
  * 显示期间再次按键：立即重新发RF，切换到下一模式并重新计5s
  *
  * 硬件：
  *   KEY3  P15 --- VCC   按下=高电平，内部下拉
  *   LED共阳，低电平点亮：LED_R=P05 / LED_G=P01 / LED_B=P06
  *   引脚开高驱动（P0_DRV）+ 关上拉，最大化灌电流
  *
  * 软件PWM：每帧10ms（10片×1ms），分辨率0~10
  ******************************************************************************
  */

#ifndef __FOGIC_TXD_C__
#define __FOGIC_TXD_C__

#include "include.h"
#if( FOGIC_PROG_TYP == FOGIC_PROG_TXD )

/* ============================================================================
   PWM参数
   ============================================================================ */
#define PWM_MAX     10          /* PWM最大亮度，每帧10ms */
#define PWM_STEP_MS 1           /* 每时间片1ms            */

/* 5秒 = 5000ms / 10ms每帧 = 500帧 */
#define SHOW_FRAMES 500

/* ============================================================================
   LED控制宏（共阳：低有效，引脚拉低=点亮）
   ============================================================================ */
#define SET_R(on)     do{ P05 = (on) ? 0 : 1; }while(0)   /* R -> P05, 共阳低有效 */
#define SET_G(on)     do{ P01 = (on) ? 0 : 1; }while(0)   /* G -> P01, 共阳低有效 */
#define SET_B(on)     do{ P06 = (on) ? 0 : 1; }while(0)   /* B -> P06, 共阳低有效 */
#define LED_ALL_OFF() do{ P05=1; P01=1; P06=1; }while(0)

/* ============================================================================
   全局变量（与原始完全一致）
   ============================================================================ */
u8  RFM_Buf[ FOGIC_PAYLOAD_LEN_MAX ];

u8  P0_MD0_Reg, P0_MD1_Reg;
u8  P1_MD0_Reg, P1_MD1_Reg;
u8  P0_AF0_Reg, P0_AF1_Reg;
u8  P1_AF0_Reg, P1_AF1_Reg;
u8  P0_PU_Reg,  P1_PU_Reg;

/* 当前模式索引 0~7，跨休眠保持 */
static u8 g_mode = 0;

/* 按键中断标志（在pwm_frame内置位） */
static u8 g_rekey = 0;

/* ============================================================================
   RF相关前向声明
   ============================================================================ */
static void Fogic_RF_Send( u8 key_func );

/* ============================================================================
   渲染一帧（软件PWM，10ms）
   同时检测按键：若P15持续按下超过消抖时间则置g_rekey
   ============================================================================ */
static void pwm_frame( u8 r, u8 g, u8 b )
{
    u8 t;
    for( t = 0; t < PWM_MAX; t++ )
    {
        SET_R( t < r );
        SET_G( t < g );
        SET_B( t < b );
        delay_ms( PWM_STEP_MS );

        if( P15 == 1 )
        {
            /* 消抖：持续检测80ms，全部为高电平才确认 */
            u8 d;
            for( d = 0; d < 80; d++ )
            {
                delay_ms( 1 );
                if( P15 == 0 ) return;  /* 中途松开，判为抖动，忽略 */
            }
            /* 确认为有效按键：等待松开后再响应 */
            while( P15 == 1 ) { delay_ms( 5 ); }
            delay_ms( 50 );             /* 松手后再等50ms防弹跳     */
            LED_ALL_OFF();
            g_rekey = 1;
            return;
        }
    }
}

/* ============================================================================
   线性插值辅助（避免重复代码）
   从 a 渐变到 b，当前步 i，总步数 steps
   ============================================================================ */
static u8 lerp( u8 a, u8 b, u8 i, u8 steps )
{
    if( b >= a ) return (u8)( a + (u16)(b - a) * i / steps );
    else         return (u8)( a - (u16)(a - b) * i / steps );
}

/* ============================================================================
   渐变一段：从(r0,g0,b0)到(r1,g1,b1)，共steps帧
   返回实际执行的帧数（被按键中断则提前返回）
   ============================================================================ */
static u8 do_fade( u8 r0, u8 g0, u8 b0,
                   u8 r1, u8 g1, u8 b1,
                   u8 steps )
{
    u8 i;
    for( i = 0; i < steps; i++ )
    {
        if( g_rekey ) return i;
        pwm_frame( lerp(r0,r1,i,steps),
                   lerp(g0,g1,i,steps),
                   lerp(b0,b1,i,steps) );
    }
    return steps;
}

/* ============================================================================
   8种颜色定义（R,G,B，亮度0~10）
   ============================================================================ */
/* 七彩8色表 */
static const u8 code COLORS[8][3] = {
    {10, 0, 0},   /* 0 红   */
    {10, 4, 0},   /* 1 橙   */
    { 8, 8, 0},   /* 2 黄   */
    { 0,10, 0},   /* 3 绿   */
    { 0, 8, 6},   /* 4 青   */
    { 0, 0,10},   /* 5 蓝   */
    { 5, 0,10},   /* 6 紫   */
    {10, 0, 5},   /* 7 品红 */
};

/* ============================================================================
   模式0~7：单色常亮（显示5秒）
   ci = COLORS表索引（0~7对应红/橙/黄/绿/青/蓝/紫/品红）
   ============================================================================ */
static void mode_solid( u8 ci )
{
    u16 total;
    for( total = 0; total < SHOW_FRAMES && !g_rekey; total++ )
    {
        pwm_frame( COLORS[ci][0], COLORS[ci][1], COLORS[ci][2] );
    }
}

/* ============================================================================
   模式8：白色呼吸（亮度0→满→0，每圈100帧=1s，5秒约5圈）
   ============================================================================ */
static void mode_breath_white(void)
{
    u16 total;
    u8 v, i;
    total = 0;
    while( total < SHOW_FRAMES && !g_rekey )
    {
        /* 上升50帧 */
        for( i = 0; i < 50 && total < SHOW_FRAMES && !g_rekey; i++, total++ )
        {
            v = (u8)(i / 5);    /* 0~9 */
            pwm_frame(v, v, v);
        }
        /* 下降50帧 */
        for( i = 0; i < 50 && total < SHOW_FRAMES && !g_rekey; i++, total++ )
        {
            v = (u8)((49 - i) / 5);
            pwm_frame(v, v, v);
        }
    }
}

/* ============================================================================
   模式9：彩色呼吸（每种颜色各呼吸一次，每次100帧=1s）
   5秒 = 5种颜色各呼吸一次
   ============================================================================ */
static void mode_breath_color(void)
{
    u8 c, i, v;
    u16 total;
    total = 0;
    c = 0;
    while( total < SHOW_FRAMES && !g_rekey )
    {
        /* 上升50帧 */
        for( i = 0; i < 50 && total < SHOW_FRAMES && !g_rekey; i++, total++ )
        {
            v = (u8)(i / 5);
            pwm_frame(
                (u8)((u16)COLORS[c][0] * v / 10),
                (u8)((u16)COLORS[c][1] * v / 10),
                (u8)((u16)COLORS[c][2] * v / 10)
            );
        }
        /* 下降50帧 */
        for( i = 0; i < 50 && total < SHOW_FRAMES && !g_rekey; i++, total++ )
        {
            v = (u8)((49 - i) / 5);
            pwm_frame(
                (u8)((u16)COLORS[c][0] * v / 10),
                (u8)((u16)COLORS[c][1] * v / 10),
                (u8)((u16)COLORS[c][2] * v / 10)
            );
        }
        c = (c + 1) % 8;
    }
}

/* ============================================================================
   模式10：全彩渐变（8色顺序平滑过渡一整圈，每段约63帧，8段≈500帧）
   ============================================================================ */
static void mode_rainbow_fade(void)
{
    u8 c, nc;
    u16 total;
    total = 0;
    c = 0;
    while( total < SHOW_FRAMES && !g_rekey )
    {
        nc = (c + 1) % 8;
        total += do_fade(
            COLORS[c][0],  COLORS[c][1],  COLORS[c][2],
            COLORS[nc][0], COLORS[nc][1], COLORS[nc][2],
            63
        );
        c = nc;
    }
}

/* ============================================================================
   显示当前模式（5秒）
   ============================================================================ */
static void LED_ShowMode(void)
{
    switch( g_mode )
    {
        case  0: mode_solid(0);        break;  /* 常亮-红   */
        case  1: mode_solid(1);        break;  /* 常亮-橙   */
        case  2: mode_solid(2);        break;  /* 常亮-黄   */
        case  3: mode_solid(3);        break;  /* 常亮-绿   */
        case  4: mode_solid(4);        break;  /* 常亮-青   */
        case  5: mode_solid(5);        break;  /* 常亮-蓝   */
        case  6: mode_solid(6);        break;  /* 常亮-紫   */
        case  7: mode_solid(7);        break;  /* 常亮-品红 */
        case  8: mode_breath_white();  break;  /* 白色呼吸  */
        case  9: mode_breath_color();  break;  /* 彩色呼吸  */
        case 10: mode_rainbow_fade();  break;  /* 全彩渐变  */
        default: break;
    }
    LED_ALL_OFF();
}

/* ============================================================================
   RF初始化（与原始完全一致）
   ============================================================================ */
static void Fogic_RFM_Init( void )
{
    rf_init();
    rf_spi_write_byte( REG_BANK0_FEATURE,   0x10 );
    rf_spi_write_byte( REG_BANK0_EN_AA,     0x00 );
    rf_spi_write_byte( REG_BANK0_CONFIG,    0xFA );
    rf_spi_write_byte( REG_BANK0_RX_PW_P0, FOGIC_RF_PWP0 );
    rf_spi_write_byte( REG_BANK0_RF_CH,     FOGIC_RF_CHANNEL );
    rf_spi_write_byte( REG_BANK0_EN_RXADDR, 0x03 );
    rf_flush_tx();
    rf_flush_rx();
    rf_clear_all_irq();
    rf_ce_low();
}

static void Fogic_RFM_BufInit( void )
{
    u8 i;
    RFM_Buf[0] = 'F';
    RFM_Buf[1] = 0;
    for( i = 2; i < 14; i++ ) RFM_Buf[i] = 0x00;
    RFM_Buf[14] = KEY_NO_FUNC;
    RFM_Buf[15] = 0x00;
}

static void Fogic_RF_Send( u8 key_func )
{
    u8 i;
    RFM_Buf[1]++;
    RFM_Buf[14] = key_func | 0x10;
    RFM_Buf[15] = Fogic_GetCRC( RFM_Buf, FOGIC_PAYLOAD_LEN_END );

    for( i = 0; i < FOGIC_RESEND_CNTS_MAX; i++ )
    {
        rf_flush_tx();
        rf_clear_all_irq();
        rf_send_pack( CMD_W_TX_PAYLOAD, RFM_Buf, FOGIC_PAYLOAD_LEN_MAX );
        rf_ce_high();
        delay_ms( 2 );
        rf_ce_low();
        rf_flush_tx();
        rf_clear_all_irq();
        delay_ms( 2 );
    }
}

/* ============================================================================
   GPIO初始化（与原始完全一致）
   ============================================================================ */
static void GPIO_Init( void )
{
    P0_MD0 &= ~0x0C;
    P0_MD0 |=  0x04;

    P0_MD1 &= ~0x3C;
    P0_MD1 |=  0x14;

    P1_MD0 &= ~0xC0;
    P1_PU  &= ~0x20;
    P1_PD  |=  0x20;

    /* 共阳LED灌电流最大化：
       - 关掉LED脚上拉，避免抬高VOL
       - P0_DRV 对应位置1，开启P01/P05/P06高驱动（最大灌电流） */
    P0_PU  &= ~((1<<1) | (1<<5) | (1<<6));
    P0_DRV |=  ((1<<1) | (1<<5) | (1<<6));

    LED_ALL_OFF();

    P0_MD0_Reg = P0_MD0; P0_MD1_Reg = P0_MD1;
    P1_MD0_Reg = P1_MD0; P1_MD1_Reg = P1_MD1;
    P0_PU_Reg  = P0_PU;  P1_PU_Reg  = P1_PU;
    P0_AF0_Reg = P0_AF0; P0_AF1_Reg = P0_AF1;
    P1_AF0_Reg = P1_AF0; P1_AF1_Reg = P1_AF1;
}

/* ============================================================================
   进入SLEEP（与原始完全一致）
   ============================================================================ */
static void Enter_Sleep( void )
{
    LED_ALL_OFF();

    rf_spi_write_byte( REG_BANK0_FEATURE, 0x20 );
    rf_spi_write_byte( REG_BANK0_PMU_CTL, 0xA9 );
    delay_ms( 2 );

    P0_MD0 = 0xFF; P0_MD1 = 0xFF;
    P1_MD0 = 0xFF; P1_MD1 = 0xFF;

    P1_MD1 &= ~0x0C;
    P1_MD1 |=  0x08;
    P1_AF1 |=  0x0C;
    P1_PD  |=  0x20;

    WKPND     = 0xFF;
    WKUP_CON0 = 0x02;
    LP_CON   |= 0x40;

    __ENTER_SLEEP_MODE;

    /* ===== 唤醒后继续 ===== */
    __SLEEP_AFTER_WAKE_UP;

    P0_MD0 = P0_MD0_Reg; P0_MD1 = P0_MD1_Reg;
    P1_MD0 = P1_MD0_Reg; P1_MD1 = P1_MD1_Reg;
    P0_PU  = P0_PU_Reg;  P1_PU  = P1_PU_Reg;
    P0_AF0 = P0_AF0_Reg; P0_AF1 = P0_AF1_Reg;
    P1_AF0 = P1_AF0_Reg; P1_AF1 = P1_AF1_Reg;

    rf_spi_init();
    Fogic_RFM_Init();

    /* 唤醒后消抖：等待按键稳定高电平100ms才算有效 */
    delay_ms( 50 );
    {
        u8 d;
        for( d = 0; d < 100; d++ )
        {
            delay_ms( 1 );
            if( P15 == 0 ) { Enter_Sleep(); return; } /* 抖动，重新休眠 */
        }
    }
    /* 等待按键完全松开 */
    while( P15 == 1 ) { delay_ms( 5 ); }
    delay_ms( 80 );                 /* 松手后再等80ms防弹跳 */
}

/* ============================================================================
   主函数
   流程：
     上电 -> 休眠
     唤醒（按键）->  g_mode切换 -> 立即发RF -> 显示5s -> 休眠
     显示中再次按键 -> 立即切换到下一模式 -> 重新发RF -> 重新计5s
   ============================================================================ */
void main( void )
{
    system_init();
    rf_spi_init();

    GPIO_Init();
    Fogic_RFM_Init();
    Fogic_RFM_BufInit();
    LED_ALL_OFF();

    Enter_Sleep();              /* 上电休眠，等待首次按键 */

    while( 1 )
    {
        g_rekey = 0;                        /* 清中断标志           */
        g_mode  = (g_mode + 1) % 11;       /* 切换到下一模式（0~10循环）*/
        Fogic_RF_Send( KFN_GO_STOP );       /* 立即发送RF           */
        LED_ShowMode();                     /* 显示当前模式5秒      */

        if( g_rekey )
        {
            /* 显示期间被按键中断：不休眠，直接切换下一模式 */
            continue;
        }

        /* 5秒正常显示完毕，进入休眠 */
        Enter_Sleep();
    }
}

/* ============================================================================ */
#endif
#endif /* __FOGIC_TXD_C__ */