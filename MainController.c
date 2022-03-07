/*
 * File:   MainController.c
 * Author: motad
 *
 * Created on 25 de febrero de 2022, 11:21 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF       // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
//----------------------Librerias--------------------------------

#include <stdint.h>
#include <pic16f887.h>
#include <xc.h>
#include "UART.h" //Librería basada en ejemplo visto en Progrmación de Microcontroladores
#include "I2C2.h"
#include "LCD.h" //Librería otorgada en Digital II
#include "MPU60502.h" //Librería realizada por Khaled , consultado el 28/02/2022
                      //Link: https://deepbluembedded.com/mpu6050-with-microchip-pic-accelerometer-gyroscope-interfacing-with-pic/  
//-------------------------Constantes------------------------------
#define _XTAL_FREQ 8000000
#define _8MHz  4
#define trans  1
#define recep  1
/*
 * ----------------------------- Variables ----------------------------------
 */
uint8_t sem;
char variable; 
uint8_t cont;
/*
 * -------------------------- Prototipos de funciones -----------------------
 */
void setup(void); //configuraciones

/*
 * ------------------------------Interupciones----------------------------
 */
void __interrupt () isr (void)
{
    if(PIR1bits.RCIF)
    {
        variable = RCREG;          
        PIR1bits.RCIF = 0;
        
    }
    return;
}

/*
 * ----------------------------- MAIN CONFIGURACION --------------------------- 
 */
void main (void)
{
    //setup de ADC, USART y demás
    setup();
    USART_setup(_8MHz,trans,recep);
    MPU6050_Init();
 
    //LCD
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Temp:  |Semaforo");
    Lcd_Set_Cursor(2,8);
    Lcd_Write_String("|");
    
    
    while(1)
    {
        //sensor();//Recibir datos de la temperatura
        //---------------I2C---------------------
        I2C_Master_Start();
        I2C_Master_Write(0x50);
        I2C_Master_Write(cont);
        I2C_Master_Stop();
        __delay_ms(200);
        
        MPU6050_Read();
        __delay_ms(1000);
        
       switch(variable)
        {
            case('1'):
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String("        ");
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String("Verde");
                cont = 1;
                break;
            case('2'):
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String("Amarillo");
                cont = 2;
                break;
            case('3'):
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String("        ");
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String("Rojo");
                cont = 3;
                break;
        } 
        
    }
}

/*
 * -------------------------------- Funciones --------------------------------
 */
void setup(void)
{
    //Configuración entradas y salida
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    I2C_Master_Init(100000);
    
    //------------------ Configuración de las interrupciones  -----------------
    INTCONbits.GIE = 1; //habilitar interrupciones
    INTCONbits.PEIE = 1;        //habilitar int perifericas
    
    //Configuracion del oscilador 
    OSCCONbits.IRCF = 0b0111; // 8MHz
    OSCCONbits.SCS = 1;
     
    return; 
}
