 /**********************************************************
 功能：75MHz时钟， AD9834正弦波点频输出，范围0-30M(可编辑0-100M)，
            方波：50k-5M，三角波：0-10M，扫频默认正弦波 。
 接口：控制接口请参照AD9834.h
 时间：2022/10/08
 版本：0.9
 作者：康威电子（原作者），MALossov（修改）

 **********************************************************/

#include "AD9834.h" 

/**
 * @brief 向AD9834写入16位数据
 * @note None
 * @param data：要写入的16位数据
 * @retval None
 */
void AD9834_Write_16Bits(unsigned int data)
{
    unsigned char i = 0;

    AD9834_SCLK_SET;
    AD9834_FSYNC_CLR;

    for (i = 0;i < 16;i++)
    {
        if (data & 0x8000)
            AD9834_SDATA_SET;
        else
            AD9834_SDATA_CLR;

        AD9834_SCLK_CLR;
        data <<= 1;
        AD9834_SCLK_SET;
    }
    AD9834_SDATA_SET;
    AD9834_FSYNC_SET;
}

/**
 * @brief 写控制位控制
 * @note IOUT正弦波 ，SIGNBITOUT方波 ，写FREQREG0 ，写PHASE0
 * @note ad9834_write_16bit(0x2028)   一次性写FREQREG0
 * @note ad9834_write_16bit(0x0038)   单独改写FREQREG0的LSB
 * @note ad9834_write_16bit(0x1038)   单独改写FREQREG0的MSB
 * @note --------------------------------------------------
 * @note IOUT三角波 ，写PHASE0
 * @note ad9834_write_16bit(0x2002)   一次性写FREQREG0
 * @note ad9834_write_16bit(0x0002)   单独改写FREQREG0的LSB
 * @note ad9834_write_16bit(0x1008)   单独改写FREQREG0的MSB
 * @note 大部分情况下，只需要写入一次，除非要改变波形类型。
          同样可以通过改变控制位来改变波形类型，但是这样会导致频率和相位的改变。
 * @param initdata:要输入的命令
 * @retval None
 */
void AD9834_Select_Wave(unsigned int initdata)

{

    AD9834_FSYNC_SET;
    AD9834_SCLK_SET;
    AD9834_RESET_SET;
    AD9834_RESET_SET;
    AD9834_RESET_CLR;

    AD9834_Write_16Bits(initdata);

}

/**
 * @brief 初始化
 * @note 将FS和PS引脚设置为低
 * @param None
 * @retval None
 */
void AD9834_Init()
{
    GPIO_ResetBits(AD9834_Control_Port, AD9834_FS);
    GPIO_ResetBits(AD9834_Control_Port, AD9834_PS);
}

/**
 * @brief 设置频率值
 * @note 频率换算公式：(Freq_value(value)=Freq_data(data)*FCLK/2^28)/(Freq_value=Freq_data*75/2^28)
 * @param freq_number：要写入的频率寄存器(FREQ_0/FREQ_1)
 * @param freq：频率值（Hz）
 * @retval None
 */
void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq)
{
    unsigned long FREQREG = (unsigned long)(268435456.0 / AD9834_SYSTEM_COLCK * freq);
    unsigned int FREQREG_LSB_14BIT = (unsigned int)FREQREG;
    unsigned int FREQREG_MSB_14BIT = (unsigned int)(FREQREG >> 14);

    if (freq_number == FREQ_0)
    {
        FREQREG_LSB_14BIT &= ~(1U << 15);
        FREQREG_LSB_14BIT |= 1 << 14;
        FREQREG_MSB_14BIT &= ~(1U << 15);
        FREQREG_MSB_14BIT |= 1 << 14;
    }
    else
    {
        FREQREG_LSB_14BIT &= ~(1 << 14);
        FREQREG_LSB_14BIT |= 1U << 15;
        FREQREG_MSB_14BIT &= ~(1 << 14);
        FREQREG_MSB_14BIT |= 1U << 15;
    }
    AD9834_Write_16Bits(FREQREG_LSB_14BIT);
    AD9834_Write_16Bits(FREQREG_MSB_14BIT);
}
//============================================================// 
/*   注：使用时先调用初始化函数初始化IO配置，然后调用AD9834_Select_Wave()选择（基本的三种波形为频率寄存器0）
波形，最后选择频率值即可输出。 */



