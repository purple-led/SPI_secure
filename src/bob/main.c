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
	/* Init srand(RANDOM_VALUE_FROM_NOISE) */
	srand_init();
	
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
	sei(); // All interrupts are enabled.
	_delay_ms(1500);
	lcd_clr();
}

int main()
{
	uint8_t pkg_head = 0, key[16];
	uint8_t lcd_buf[80], pkg_buf[64];
	int16_t lcd_ampl = 0;
	
	memset(key, 0, 16 * sizeof(uint8_t));

	init();
	
	/* Click to receive */
	lcd_clr();
	lcd_write((uint8_t *) "Click to receive");
	lcd_curs(1, 0);
	lcd_write((uint8_t *) "key by DiffHell");
	_delay_ms(500);

	/* Getting private key by Diffie-Hellman algorithm with tricky method */
	set_false(PORTD, 7);
	
	difhel_private_key(key, (uint16_t) 128);
	aes128_init(key, &ctx);
	set_true(PORTD, 7)
;
	/* Key was received */
	lcd_clr();
	lcd_write((uint8_t *) "Key was received");
	lcd_curs(1, 0);
	lcd_write((uint8_t *) "Click on button");
	_delay_ms(500);
/*
	int i = 0;

	for(i = 0; i < 4; i ++)
	{
		sprintf(lcd_buf, "(%d)(%lu)", i, get_part_key(key, i));
		lcd_clr();
		lcd_write(lcd_buf);
		_delay_ms(3000);
	}
*/		
	/* Getting encrypted data and decoding */
	while(1)
	{
		set_false(PORTD, 7);
		receive_package(pkg_buf, &pkg_head, &ctx);
		set_true(PORTD, 7);

		if(!read_bit(pkg_head, 5))
		{
			sprintf(lcd_buf, "%s", pkg_buf);
		
			lcd_clr();
			lcd_write(lcd_buf);

			lcd_ampl = strlen((char *) lcd_buf) - 16;
			if(lcd_ampl > 0) lcd_there_back(0, lcd_ampl, 1);
			_delay_ms(50);
		}
	}
	
	return 0;
}

