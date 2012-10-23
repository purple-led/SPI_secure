/**
* Bob
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "iface.h"
#include "spi.h"
#include "lcd.h"
#include "aes.h"

#define BLOCK_SIZE 16

uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };

uint8_t data[BLOCK_SIZE]; 

aes128_ctx_t ctx;
void init(void)
{
	/* For LCD */
	set_true(DDRA, LCD_RS); //RC is output
	set_true(DDRA, LCD_RW); //RW is output
	set_true(DDRA, LCD_EN); //EN is output
	PORTA = 0x00; //zero in output
	DDRC = 0xff; //PD0-7 are output
	PORTC = 0x00; //zero in output	
	lcd_init();
	lcd_com(0x0C);
	lcd_curs(0, 0);
	lcd_write("   SPI secure   ");
	lcd_curs(1, 0);
	_delay_ms(15000);
	lcd_clr();
	
	spi_init_slave();
	
	aes128_init(key, &ctx);
}

int main(void)
{
	int need_to_recv = 1;
  	char lcdbuf[16];
 
	init();
 
	while(0)
	{
		if (need_to_recv)
		{
			lcd_clr();
			for (int i = 0; i < BLOCK_SIZE; i ++) data[i] = spi_sync(0);
			aes128_dec(data, &ctx);
			sprintf(lcdbuf, "data = [%3d]", data[7]);
			lcd_write(lcdbuf);
			need_to_recv = 0;
		}
		_delay_ms(500);
	}
	
	return 0;
}

