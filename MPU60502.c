
#include <xc.h>
#include "I2C2.h"
#include "MPU60502.h"
#include "LCD.h"
#include "UART.h"
#include <stdio.h>

//-------------[ MPU6050 Routines ]------------------
//---------------------------------------------------

void MPU6050_Init() {
  // Power-Up Delay & I2C_Init
  __delay_ms(100);
  //I2C_Master_Init(100000);
  
  // Setting The Sample (Data) Rate
  I2C_Start(0xD0);
  I2C_Master_Write(SMPLRT_DIV);
  I2C_Master_Write(0x07);
  I2C_Master_Stop();
 
  // Setting The Clock Source
  I2C_Start(0xD0);
  I2C_Master_Write(PWR_MGMT_1);
  I2C_Master_Write(0x01);
  I2C_Master_Stop();
 
  // Configure The DLPF
  I2C_Start(0xD0);
  I2C_Master_Write(CONFIG);
  I2C_Master_Write(0x00);
  I2C_Master_Stop();
 
  // Configure The ACCEL (FSR= +-2g)
  I2C_Start(0xD0);
  I2C_Master_Write(ACCEL_CONFIG);
  I2C_Master_Write(0x00);
  I2C_Master_Stop();
 
  // Configure The GYRO (FSR= +-2000d/s)
  I2C_Start(0xD0);
  I2C_Master_Write(GYRO_CONFIG);
  I2C_Master_Write(0x18);
  I2C_Master_Stop();
 
  // Enable Data Ready Interrupts
  I2C_Start(0xD0);
  I2C_Master_Write(INT_ENABLE);
  I2C_Master_Write(0x01);
  I2C_Master_Stop();
}

void MPU6050_Read(void)
{
  int Ax,Ay,Az,Gx,Gy,Gz,T;
  //int AX, AY, AZ, GX, GY, GZ;
  int t;

 // Prepare For Reading, Starting From ACCEL_XOUT_H
  I2C_Start(0xD0);
  I2C_Master_Write(ACCEL_XOUT_H);
  I2C_Master_Stop();
  I2C_Start(0xD1);
  Ax = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Ay = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Az = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  T  = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gx = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gy = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gz = ((int)I2C_Read(0)<<8) | (int)I2C_Read(1);
  I2C_Master_Stop();
  
  //AX = (int)Ax/16384.0;
  //AY = (int)Ay/16384.0;
  //AZ = (int)Az/16384.0;
  //GX = (int)Gx/131.0 * 100;
  //GY = (int)Gy/131.0 * 100;
  //GZ = (int)Gz/131.0 * 100;
  t = (((int)T / 340.00) + 36.53) * 10;

  conversion(t);
  Lcd_Set_Cursor(2,5);
  Lcd_Write_String("C");
  TX_usart(',');
}
void conversion (int temp) {
    int tenths, units, tens;
    int etenths, eunits, etens;
    tenths = (temp%10);
    units = (temp%100 - tenths)/10;
    tens = (temp - units*10 - tenths)/100;
    
    etenths = tenths + 48;
    eunits = units + 48;
    etens = tens + 48;
    
    Lcd_Set_Cursor(2,1);
    Lcd_Write_Char(etens);
    TX_usart(etens);
    
    Lcd_Write_Char(eunits);
    TX_usart(eunits);
    
    Lcd_Write_Char('.');
    
    Lcd_Write_Char(etenths);
    TX_usart(etenths);
}