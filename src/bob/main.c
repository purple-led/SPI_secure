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
#include "difhel.h"

aes128_ctx_t ctx;

void init()
{
	/* For LCD */
	set_true(DDRA, LCD_RS); //RC is output
	set_true(DDRA, LCD_RW); //RW is output
	set_true(DDRA, LCD_EN); //EN is output
	PORTA = 0x00; //zero in output
	DDRC = 0xff; //PC0-7 are output
	PORTC = 0x00; //zero in output	

	/* For check LED */
	set_true(DDRD, 7);
	set_true(PORTD, 7);

	lcd_init();
	lcd_com(0x0C);
	
	/* Greeting */
	lcd_curs(0, 0);
	lcd_write((uint8_t *)"  \"SPI secure\"  ");
	lcd_curs(1, 0);

	lcd_write((uint8_t *)"--___--___--___-");	
	
	/* Init to connect */
	spi_set_slave();
	
	/* Ready to work */
	set_true(SREG, 7); // All interrupts are enabled.
	_delay_ms(1500);
	lcd_clr();
}

int main()
{
	uint8_t lcd_buf[80], pkg_buf[1][64];
	uint8_t lcd_ampl = 0;
	uint8_t key[16];

	init();

	/* Getting private key by Diffie-Hellman algorithm with tricky method */
	while(0)
	{
		if(!read_bit(PINB, SS))
		{
			set_false(PORTD, PD6);			

			difhel_private_key(key, 128);
			aes128_init(key, &ctx);

			sprintf(lcd_buf, "My SPCR %x", SPCR);
			lcd_write(lcd_buf);
			set_true(PORTD, PD6);
			break;
		}
		else set_true(PORTD, PD6);

		_delay_us(50);
	}

	/* Getting encrypted data and decoding */
	while(1)
	{
		set_false(PORTD, 7);

		//aes_receive_package(pkg_buf[0], &ctx);
		receive_package(pkg_buf[0]);
			
		sprintf(lcd_buf, "%s", pkg_buf[0]);

		lcd_clr();
		lcd_write(lcd_buf);
	
		//lcd_ampl = 20;//strlen((char *) lcd_buf) - 16;
		//if(lcd_ampl) lcd_there_back(0, 16, 1 << 16);
		
		set_true(PORTD, 7);
	
		_delay_ms(5000);
	}
	
	return 0;
}

