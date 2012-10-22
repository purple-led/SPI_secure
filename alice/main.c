/**
* Alice
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_true(in, pos) in |= (1 << pos)
#define set_false(in, pos) in &= ~(1 << pos)
#define read_bit(in, pos) ((in & (1 << pos)) ? 1 : 0)

#define SS PB4
#define MOSI PB5
#define MISO PB6
#define SCK PB7

void spi_init_master()
{
	set_true(DDRB, MOSI);
	set_true(DDRB, SCK);
	set_true(DDRB, SS);
	
	set_true(SPCR, MSTR);
	set_true(SPCR, SPR0); // set SCK = OF / 16
	set_true(SPCR, SPE);  // spi enable
}

void spi_transmit_master(uint8_t data)
{
	SPDR = data;
	while(!read_bit(SPSR, SPIF));
}

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
		
		spi_transmit_master(check);
		_delay_ms(50);
	}

	return 0;
}
