#ifndef _LCD_H_
#define _LCD_H_

#include <string.h>

#define LCD_RS PA0
#define LCD_RW PA1
#define LCD_EN PA2

#define LCD_PORT_CONF PORTA
#define LCD_PORT PORTC

/* Input command in LCD */
void lcd_com(uint8_t p)
{
	set_false(LCD_PORT_CONF, LCD_RS); //LCD_RS = 0 (for command)
	set_true(LCD_PORT_CONF, LCD_EN); //LCD_EN = 1 (start to input command in LCD)

	LCD_PORT = p; //input command 
	_delay_us(100); //duration of the signal
	
	set_false(LCD_PORT_CONF, LCD_EN); //LCD_EN = 0 (finish to input command in LCD)
	if(p == 0x01 || p == 0x02) _delay_us(1800); //pause for execution
	else _delay_us(50);
}

/* Input data in LCD */
void lcd_dat(uint8_t p)
{
	set_true(LCD_PORT_CONF, LCD_RS); //LCD_RS = 1(for data)
	set_true(LCD_PORT_CONF, LCD_EN); //LCD_EN = 1 (start to input data in LCD)
	
	LCD_PORT = p; //input data
	_delay_us(100); //duration of the signal
	
	set_false(LCD_PORT_CONF, LCD_EN); //LCD_EN = 0 (finish to input data in LCD)
	_delay_us(50); //pause for execution
}

void lcd_write(char * string)
{
	int i = 0, count = strlen(string);
	for(i = 0; i < count; i ++) lcd_dat(string[i]);
}

/* The initialization function LCD */
void lcd_init()
{
	lcd_com(0x08); //display off(1640us)
	lcd_com(0x38); //8 bit, 2 lines(40us)
	lcd_com(0x01); //cleaning the display(1640us)
	lcd_com(0x06); //shift the cursor to the right(40us)
	lcd_com(0x0D); //a blinking cursor(40us)
}

/* Clearing of LCD */
void lcd_clr()
{
	lcd_com(0x01);
}

/* Moving cursor */
void lcd_curs(uint8_t line, uint8_t index)
{
	if((line == 0 || line == 1) && (0 <= index && index <= 15))
	{
		lcd_com((1 << 7) + (line << 6) + index);
	}
}

#endif // _LCD_H_
