/**
 ****************************************************************************************************
 * @file        main.c
 * @author
 * @version     V1.0
 * @date
 ****************************************************************************************************
 ****************************************************************************************************
 */
#include "main.h"

// 外部变量声明（定义在keypad.c中）
extern uint8_t keyNum[4];
extern uint8_t setn;
extern float volMax;
extern uint16_t currentMax;
extern float voltage;
extern uint16_t current;
extern char display[16];
extern u8 warningFlag;
extern u8 load;
extern uint8_t data_index;

// 电量百分比
uint8_t elQuantity = 0;

/**
 * @brief  电量计算函数
 */
void GetBatteryCapacity(void)
{
    u8 Vol;
    Vol = (u16)(voltage * 1000) / 38;
    if(Vol >= 100)
    {
        elQuantity = 100;
    }else{
        elQuantity = Vol;
    }
}

/**
 * @brief  电量柱状图显示函数
 */
void DisplayBatteryBar(void)
{
    u8 i;
    u8 barLength;

    // 计算柱状图长度 (0-10格)
    barLength = elQuantity / 10;
    if(barLength > 10) barLength = 10;

    // 显示电池外框
    OLED_ShowStr(80, 4, "[          ]", 1, 0);

    // 显示电量填充
    for(i = 0; i < barLength; i++)
    {
        OLED_ShowStr(82 + i, 4, "|", 1, 0);
    }

    // 低电量警告
    if(elQuantity < 20)
    {
        OLED_ShowStr(94, 4, "!", 1, 0);
    }
}

/**
 * @brief  初始化模块
 */
int main(void)
{
    // 系统初始化
    HAL_Init();                             /* 初始化HAL库 */
    SystemClock_Config(RCC_PLL_MUL9);       /* 设置时钟, 72Mhz */
    delay_init(72);                         /* 延时初始化 */
    
    // 外设初始化
    MX_IIC_Init();                          /* I2C初始化 */
    Uart_Init();                            /* 串口初始化为115200 */
    MX_ADC_Init();                          /* ADC初始化 */
    MX_SPIx_Init();                         /* SPI初始化 */
    MX_TIMx_Init();                         /* 定时器初始化 */
    MX_Hard_Init();                         /* 所有外设初始化 */

    // 看门狗初始化
#if(WDT_RUN)
    IWDG_init(IWDG_PRESCALER_64, 625*6);   /* 看门狗初始化 */
    IWDG_feed();                            /* 喂狗 */
#endif

    // 主循环
    while(1)
    {
        // 按键扫描
        KeyScan();
        
        // 按键处理
        KeyScanHandle();
        
        // 显示功能
        Display();
        
        // 保护功能
        CotorFun();
        
        // 喂狗
#if(WDT_RUN)
        IWDG_feed();
#endif
        
        // 延时
        HAL_Delay(50);
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
