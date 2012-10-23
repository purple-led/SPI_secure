/**
* Bob
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../include/iface.h"
#include "../include/spi.h"
#include "../include/lcd.h"

int main()
{
  	char rec_data[16];
  
	/* For LCD */
	set_true(DDRA, LCD_RS); //RC is output
	set_true(DDRA, LCD_RW); //RW is output
	set_true(DDRA, LCD_EN); //EN is output
	PORTA = 0x00; //zero in output
	DDRC = 0xff; //PD0-7 are output
	PORTC = 0x00; //zero in output
	
	lcd_init();
	
	/* Greeting */
	char * greet_1 = "  \"SPI secure\"  ";
	char * greet_2 = "      v1.0      ";

	lcd_com(0x0C);

	lcd_curs(0, 0);
	lcd_write(greet_1);
	lcd_curs(1, 0);
	lcd_write(greet_2);
	_delay_ms(1500);
	
	spi_init_slave();
	
	lcd_clr();
	
	while(1)
	{
		lcd_clr();
		sprintf(rec_data, "[%d]", spi_sync(0));
		lcd_write(rec_data);
		
		_delay_ms(50);
	}
	
	return 0;
}
