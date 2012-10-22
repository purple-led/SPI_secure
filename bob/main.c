/**
* Bob
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_true(in, pos) in |= (1 << pos)
#define set_false(in, pos) in &= ~(1 << pos)
#define read_bit(in, pos) ((in & (1 << pos)) ? 1 : 0)

void spi_init_slave()
{
	set_true(DDRB, PB6); // output pin of MISO
	set_true(SPCR, SPE); // spi enable
}

uint8_t spi_receive()
{
	SPDR = 66;
	while(!read_bit(SPSR, SPIF)); // wait for reception complete
	return SPDR;
}

int main()
{
	uint8_t check = 0;
  
	set_true(DDRC, PC0); // LED
	set_false(PORTC, PC0); // low voltage
	
	spi_init_slave();
	
	while(1)
	{
		check = spi_receive();
		
		if(check == 1) set_true(PORTC, PC0);
		else set_false(PORTC, PC0);
		
		_delay_ms(50);
	}

	return 0;
}
