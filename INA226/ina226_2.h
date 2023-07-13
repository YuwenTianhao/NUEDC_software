#ifndef INA226_DEMO_INA226_2_H
#define INA226_DEMO_INA226_2_H

#include "main.h"
#include "i2c.h"

#define     CFG_REG          0x00        
#define     SV_REG           0x01        
#define     BV_REG           0x02        
#define     PWR_REG          0x03        
#define     CUR_REG          0x04        
#define     CAL_REG          0x05        
#define     INA226_GET_ADDR  0XFF        
#define     INA226_ADDR1     0x80        
#define     ONFF_REG         0x06        
#define     AL_REG           0x07        

typedef struct
{
    float voltageVal;            //mV
    float Shunt_voltage;        //uV
    float Shunt_Current;        //mA
    float powerVal;
}INA226;

void INA226_Init(void);
void GetVoltage(float *Voltage);
long INA226_GetShunt_Current(uint8_t addr);
uint16_t INA226_GET_CAL_REG(uint8_t addr);
void Get_Shunt_voltage(float *Voltage);
void Get_Shunt_Current(float *Current);
uint8_t status();
void GetPower();
extern INA226 INA226_data;
//float GetV(void);

#endif //INA226_DEMO_INA226_2_H
