#include "ina226_2.h"
#include "tjc_usart_hmi.h"
INA226 INA226_data;

void INA226_SendData(uint8_t addr,uint8_t reg,uint16_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, addr, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, 2, HAL_MAX_DELAY);
}

void INA226_Init(void)
{
    HAL_Delay(10);
    //    INA226_SendData(INA226_ADDR1,CFG_REG,0xC000);

//    HAL_Delay(10);
    //uint16_t cfg_reg_value = 0x4000|(3<<9)|(4<<6)|(4<<3)|7;
//    INA226_SendData(INA226_ADDR1,CFG_REG,0x484f);
//    INA226_SendData(INA226_ADDR1,CAL_REG,0x0012);
    INA226_SendData(INA226_ADDR1,0x00,0x4727);
    INA226_SendData(INA226_ADDR1,0x05,0x800);//A00
}

long INA226_GetShunt_Current(uint8_t addr)
{
    long temp=0;
    HAL_I2C_Mem_Read(&hi2c1, addr, CUR_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &temp, 2, HAL_MAX_DELAY);
    if(temp&0x8000)
    {
        temp = ~(temp - 1);
        temp = (uint16_t)temp - 2 * (uint16_t)temp;
    }
    return temp;
}

uint16_t  INA226_Get_ID(uint8_t addr)
{
    uint32_t temp=0;
    HAL_I2C_Mem_Read(&hi2c1, addr, INA226_GET_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &temp, 2, HAL_MAX_DELAY);
    return (uint16_t)temp;
}


uint16_t INA226_GET_CAL_REG(uint8_t addr)
{
    uint32_t temp=0;
    HAL_I2C_Mem_Read(&hi2c1, addr, CAL_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &temp, 2, HAL_MAX_DELAY);
    return (uint16_t)temp;
}

//1.25mV/bit
uint32_t INA226_GetVoltage(uint8_t addr)
{
    uint32_t temp=0;
    HAL_I2C_Mem_Read(&hi2c1, addr, BV_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &temp, 2, HAL_MAX_DELAY);
    return (uint32_t)temp;
}

//2.5uV/bit
uint32_t INA226_GetShuntVoltage(uint8_t addr)
{
    uint32_t temp=0;
    HAL_I2C_Mem_Read(&hi2c1, addr, SV_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &temp, 2, HAL_MAX_DELAY);
    if(temp&0x8000)    temp = ~(temp - 1);
    return (uint32_t)temp;
}


uint16_t INA226_Get_Power(uint8_t addr)
{
    int16_t temp=0;
    HAL_I2C_Mem_Read(&hi2c1, addr, PWR_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &temp, 2, HAL_MAX_DELAY);
    return (uint16_t)temp;
}


void GetVoltage(float *Voltage)//mV
{
    Voltage[0] = INA226_GetVoltage(INA226_ADDR1)*1.25f;
//    printf("ina226 v = %.2f\r\n", Voltage[0]);
}


void Get_Shunt_voltage(float *Voltage)//uV
{
    Voltage[0] = (INA226_GetShuntVoltage(INA226_ADDR1)*2.2);

}


void Get_Shunt_Current(float *Current)//mA
{
    Current[0] = (INA226_GetShunt_Current(INA226_ADDR1)* 2.500f);
}


void GetPower()//W
{
    GetVoltage(&INA226_data.voltageVal);            //mV
    Get_Shunt_voltage(&INA226_data.Shunt_voltage);    //uV
    Get_Shunt_Current(&INA226_data.Shunt_Current);    //mA
    INA226_data.powerVal=INA226_data.voltageVal*0.001f * INA226_data.Shunt_Current*0.001f;
}
/**
* @brief 读取串口屏的返回值
* @return 0xff为无效值，0x03为初始页面，0x01为电流检测页面, 0x02为电压检测页面
*/
uint8_t status()
{
    uint8_t status_now;
    while(usize >= 5)
    {
//校验帧头帧尾是否匹配
        if(u(0) != 0x55 || u(2) != 0xff || u(3) != 0xff || u(4) != 0xff)
        {
//不匹配删除1字节
            udelete(1);
        }else
        {
//匹配，跳出循环
            break;
        }
    }
//进行解析
    if(usize >= 5 && u(0) == 0x55 && u(2) == 0xff && u(3) == 0xff && u(4) == 0xff)
    {
        status_now = u(1);
        udelete(5);
        return status_now;
    }
    else
    {
        return 0xff;
    }
}


