/**
* Alice
*/

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "iface.h"
#include "aes.h"
#include "spi.h"

#define BLOCK_SIZE 16

uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };

aes128_ctx_t ctx;

void init(void)
{
	/* For button */
	set_false(DDRD, PD0);
	set_true(PORTD, PD0); // gets extra resistance of 100 kOhm

	/* For check LED */
	set_true(DDRC, PC0);
	set_true(PORTC, PC0); // low voltage

	/* For aes */	
	aes128_init(key, &ctx);

	/* Init to connect */
	spi_init_master();
}

int main(void)
{
	uint8_t need_to_send = 1;
	uint8_t first_pkg[] = "Hello, everybody! I'm SPI secure!";
 
	init();
 
	while (1)
	{	
		if (!read_bit(PIND, PD0) && need_to_send)
		{
			need_to_send = 0;
			aes_send_package(first_pkg, &ctx);
			set_false(PORTC, PC0);
		}
		else set_true(PORTC, PC0);
		
		_delay_ms(50);
	}

	return 0;
}

