/**
* Alice
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../include/iface.h"
#include "../include/spi.h"

int main()
{
	uint8_t check = 0;
  
	set_false(DDRD, PD0); // button
	set_true(PORTD, PD0); // PDO gets extra resistance of 100 kOhm
	
	set_true(DDRC, PC0); // LED
	set_true(PORTC, PC0); // low voltage
	
	spi_init_master();
	
	while(1)
	{
		check = read_bit(PIND, PD0);
		
		if(check == 1) set_true(PORTC, PC0);
		else set_false(PORTC, PC0);
		
		spi_sync(check);
		_delay_ms(50);
	}

	return 0;
}
