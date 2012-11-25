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
#include "difhel.h"

aes128_ctx_t ctx;

void init()
{
	/* Init srand(RANDOM_VALUE_FROM_NOISE) */
	srand_init();
	
	/* For button */
	set_false(DDRD, 7);
	set_true(PORTD, 7); // gets extra resistance of 100 kOhm

	/* For check LED */
	set_true(DDRC, 0);
	set_true(PORTC, 0); // low voltage

	/* Init to connect */
	spi_set_master();
}

int main()
{
	uint8_t free_pkg[][40] =
	{
		"Hello, everybody! I'm SPI secure!",
		"After each click send 1 msg."     ,
		"This is the third msg from Alice.",
		"I have made this several weeks :(",
		"We need to detect time of trans.",
		"We need a keyboard to write msg."
	};
	
	uint8_t i = 0, count_free_pkg = 6;
	uint8_t key[16];

	init();

	/* Getting private key by Diffie-Hellman algorithm with tricky method */
	while(1)
	{
		if(!read_bit(PIND, 7))
		{	
			set_false(PORTC, 0);
			difhel_private_key(&key, 128);
			aes128_init(&key, &ctx);

			set_true(PORTC, 0);	
			break;
 		}
		else set_true(PORTC, 0);

		_delay_us(50);
	}

	/* Encrypting data and sending them */
	while (1)
	{
		if (!read_bit(PIND, 7))
		{
			set_false(PORTC, 0);

			aes_send_package(free_pkg[i], &ctx);

			i = (i + 1) % count_free_pkg;
		}
		else set_true(PORTC, 0);
		
		_delay_ms(100);
	}

	return 0;
}

