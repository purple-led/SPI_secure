/**
* Bob
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "iface.h"
#include "lcd.h"
#include "aes.h"
#include "spi.h"

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
	DDRC = 0xff; //PC0-7 are output
	PORTC = 0x00; //zero in output	

	lcd_init();
	lcd_com(0x0C);
	
	/* For check LED */
	set_true(DDRD, 6);
	set_true(PORTD, 6);

	/* Greeting */
	lcd_curs(0, 0);
	lcd_write((uint8_t *)"  \"SPI secure\"  ");
	lcd_curs(1, 0);

	lcd_write((uint8_t *)"--___--___--___-");	
	_delay_ms(1500);

	/* For aes */
	aes128_init(key, &ctx);
	
	/* Init to connect */
	spi_init_slave();
	
	/* Ready to work  */
	lcd_clr();
}

int main(void)
{
	uint8_t need_to_recv = 1, lcd_buf[32], pkg_buf[100];
 
	init();
 
	while(1)
	{
		if (need_to_recv)
		{
			need_to_recv = 0;
				
			//receive_package(pkg_buf);
			aes_receive_package(pkg_buf, &ctx);
			set_false(PORTD, PD6);

			sprintf((char *) lcd_buf, "[%s]", (char *) pkg_buf);
			lcd_clr();
			lcd_write(lcd_buf);
		}
		_delay_ms(50);
	}
	
	return 0;
}

