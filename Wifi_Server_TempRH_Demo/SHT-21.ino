#include <Wire.h>

enum SHT_Commands {
    READ_REGISTER = 0xE7,
    WRITE_REGISTER = 0xE6,
    READ_TEMP_HOLD_MASTER = 0xE3,
    READ_RH_HOLD_MASTER = 0xE5,
    READ_TEMP_NO_HOLD = 0xF3,
    READ_RH_NO_HOLD = 0xF5,
    SOFT_RESET = 0xFE,
};

static uint8_t SHT21_ADDRESS = 0x40;

void SHT_SoftReset() {
    Wire.beginTransmission(SHT21_ADDRESS);
    Wire.write(SOFT_RESET);
    Wire.endTransmission();
    delay(15);
}

static unsigned short SHT_ReadTemp() {
    uint8_t temp[3] = {0xFF, 0xFF, 0xFF}; //16 bits plus checksum
    
    Wire.beginTransmission(SHT21_ADDRESS);
    Wire.write(READ_TEMP_NO_HOLD);
    Wire.endTransmission();
    
    while(Wire.requestFrom(SHT21_ADDRESS, (uint8_t)3) < 3)
    {
        delay(1);
    }
    
    for(int i=0; i<3 && Wire.available(); i++)
    { 
        temp[i] = Wire.read();
    }
    
    return (temp[0]<<8) | temp[1];
}

static unsigned short SHT_ReadRH() {
    uint8_t rh[3] = {0xFF, 0xFF, 0xFF}; //16 bits plus checksum
    Wire.beginTransmission(SHT21_ADDRESS);
    Wire.write(READ_RH_NO_HOLD);
    Wire.endTransmission();
    
    while(Wire.requestFrom(SHT21_ADDRESS, (uint8_t)3) < 3)
    {
        delay(1);
    }
    
    for(int i=0; i<3 && Wire.available(); i++)
    { 
        rh[i] = Wire.read();
    }
    return (rh[0]<<8) | rh[1];
}

//==============================================================================
float SHT2x_CalcRH(unsigned short u16sRH)
//==============================================================================
{
     float humidityRH; // variable for result
     u16sRH &= ~0x0003; // clear bits [1..0] (status bits)
     //-- calculate relative humidity [%RH] --
     humidityRH = -6.0 + 125.0/65536 * (float)u16sRH; // RH= -6 + 125 * SRH/2^16
     return humidityRH;
}
//==============================================================================
float SHT2x_CalcTemperatureC(unsigned short  u16sT)
//==============================================================================
{
     float temperatureC; // variable for result
     u16sT &= ~0x0003; // clear bits [1..0] (status bits)
     //-- calculate temperature [°C] --
     temperatureC= -46.85 + 175.72/65536 *(float)u16sT; //T= -46.85 + 175.72 * ST/2^16
     return temperatureC;
}

