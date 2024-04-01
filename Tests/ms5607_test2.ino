//!
//! @file an520_SPI.c,v
//!
//! Copyright (c) 2009 MEAS Switzerland
//!
//!
//!
//! @brief This C code is for starter reference only. It is written for the
//! MEAS Switzerland MS56xx pressure sensor modules and Atmel Atmega644p
//! microcontroller.
//!
//! @version 1.0 $Id: an520_SPI.c,v 1.0
//!
//! @todo
//_____ M A C R O S
#define TRUE 1
#define FALSE 0
#define F_CPU 4000000UL // 4 MHz XTAL
#define CMD_RESET 0x1E // ADC reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command
#define CMD_ADC_D1 0x00 // ADC D1 conversion
#define CMD_ADC_D2 0x10 // ADC D2 conversion
#define CMD_ADC_256 0x00 // ADC OSR=256
#define CMD_ADC_512 0x02 // ADC OSR=512
#define CMD_ADC_1024 0x04 // ADC OSR=1024
#define CMD_ADC_2048 0x06 // ADC OSR=2056
#define CMD_ADC_4096 0x08 // ADC OSR=4096
#define CMD_PROM_RD 0xA0 // Prom read command
#define csb_hi() digitalWrite(5,LOW) // setting CSB low
#define csb_lo() digitalWrite(5,HIGH) // setting CSB high
//_____ I N C L U D E S
#include <stdio.h>
#include "libraries/util/delay.h"
#include "libraries/avr/io.h"
#include <math.h>
//_____ D E F I N I T I O N S
void spi_send(char cmd);
void cmd_reset(void);
unsigned long cmd_adc(char cmd);
unsigned int cmd_prom(char coef_num);
unsigned char crc4(unsigned int n_prom[]);
//********************************************************
//! @brief send 8 bit using SPI hardware interface
//!
//! @return 0
//********************************************************
void spi_send(char cmd)
{
SPDR= cmd; // put the byte in the SPI hardware buffer and start sending
while (bit_is_clear(SPSR, 7)); // wait that the data is sent
}

//********************************************************
//! @brief send reset sequence
//!
//! @return 0
//********************************************************
void cmd_reset(void)
{
csb_lo(); // pull CSB low to start the command
spi_send(CMD_RESET); // send reset sequence
_delay_ms(3); // wait for the reset sequence timing
csb_hi(); // pull CSB high to finish the command
}
//********************************************************
//! @brief preform adc conversion
//!
//! @return 24bit result
//********************************************************
unsigned long cmd_adc(char cmd)
{
unsigned int ret;
unsigned long temp=0;
csb_lo(); // pull CSB low
spi_send(CMD_ADC_CONV+cmd); // send conversion command
switch (cmd & 0x0f) // wait necessary conversion time
{
case CMD_ADC_256 : _delay_us(900); break;
case CMD_ADC_512 : _delay_ms(3); break;
case CMD_ADC_1024: _delay_ms(4); break;
case CMD_ADC_2048: _delay_ms(6); break;
case CMD_ADC_4096: _delay_ms(10); break;
}
csb_hi(); // pull CSB high to finish the conversion
csb_lo(); // pull CSB low to start new command
spi_send(CMD_ADC_READ); // send ADC read command
spi_send(0x00); // send 0 to read 1st byte (MSB)
ret=SPDR;
temp=65536*ret;
spi_send(0x00); // send 0 to read 2nd byte
ret=SPDR;
temp=temp+256*ret;
spi_send(0x00); // send 0 to read 3rd byte (LSB)
ret=SPDR;
temp=temp+ret;
csb_hi(); // pull CSB high to finish the read command
return temp;
}
//********************************************************
//! @brief Read calibration coefficients
//!
//! @return coefficient
//********************************************************

unsigned int cmd_prom(char coef_num)
{
unsigned int ret;
unsigned int rC=0;
csb_lo(); // pull CSB low
spi_send(CMD_PROM_RD+coef_num*2); // send PROM READ command
spi_send(0x00); // send 0 to read the MSB
ret=SPDR;
rC=256*ret;
spi_send(0x00); // send 0 to read the LSB
ret=SPDR;
rC=rC+ret;
csb_hi(); // pull CSB high
return rC;
}
//********************************************************
//! @brief calculate the CRC code for details look into AN521
//!
//! @return crc code
//********************************************************
unsigned char crc4(unsigned int n_prom[])
{
int cnt; // simple counter
unsigned int n_rem; // crc reminder
unsigned int crc_read; // original value of the crc
unsigned char n_bit;
n_rem = 0x00;
crc_read=n_prom[7]; //save read CRC
n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
{ // choose LSB or MSB
if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
for (n_bit = 8; n_bit > 0; n_bit--)
{
if (n_rem & (0x8000))
{
n_rem = (n_rem << 1) ^ 0x3000;
}
else
{
n_rem = (n_rem << 1);
}
}
}
n_rem= (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code
n_prom[7]=crc_read; // restore the crc_read to its original place
return (n_rem ^ 0x00);
}
//********************************************************
//! @brief main program
//!
//! @return 0
//********************************************************

int main (void)
{
unsigned long D1; // ADC value of the pressure conversion
unsigned long D2; // ADC value of the temperature conversion
unsigned int C[8]; // calibration coefficients
double P; // compensated pressure value
double T; // compensated temperature value
double dT; // difference between actual and measured temperature
double OFF; // offset at actual temperature
double SENS; // sensitivity at actual temperature
int i;
unsigned char n_crc; // crc value of the prom
DDRA = 0xFE; // prepare the port A
DDRB = 0xBF; // SDO input
DDRC = 0x00; // I2C pins as input
DDRD = 0x82; // prepare the port D, RX out and TX out;
PORTA= 0x10;
PORTD= 0x20;
//SPI settings:master, mode 0, fosc/4
SPCR=(1<<SPE)|(1<<MSTR);
//alternative SPI settings: master, mode 3, fosc/4
//SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA);
cmd_reset(); // reset the module after powerup
for (i=0;i<8;i++){ C[i]=cmd_prom(i);} // read calibration coefficients
n_crc=crc4(C);
while(TRUE) // loop without stopping
{
D1=cmd_adc(CMD_ADC_D1+CMD_ADC_256); // read uncompensated pressure
D2=cmd_adc(CMD_ADC_D2+CMD_ADC_4096); // read uncompensated temperature
// calcualte 1st order pressure and temperature (MS5607 1st order algorithm)
dT=D2-C[5]*pow(2,8);
OFF=C[2]*pow(2,17)+dT*C[4]/pow(2,6);
SENS=C[1]*pow(2,16)+dT*C[3]/pow(2,7);
T=(2000+(dT*C[6])/pow(2,23))/100;
P=(((D1*SENS)/pow(2,21)-OFF)/pow(2,15))/100;
// place to use P, T, put them on LCD, send them trough RS232 interface...
}
return 0;
}