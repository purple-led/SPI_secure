/**
* Alice
*/

#include <avr/io.h>
#include <string.h>
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
	uint8_t free_pkg[][50] =
	{
		"Hello, everybody! I'm SPI secure!",
		"After each click send 1 msg."     ,
		"This is the third msg from Alice.",
		"We need a keyboard to write msg."
	};
	
	uint8_t i = 0, count_free_pkg = 4;
	uint8_t key[16];

	init();

	/* Getting private key by Diffie-Hellman algorithm with tricky method */
	while(0)
	{
		if(!read_bit(PIND, 7))
		{	
			set_false(PORTC, 0);
			difhel_private_key(&key, 128);
			aes128_init(&key, &ctx);

			set_true(PORTC, PC0);	
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
			set_false(PORTC, PC0);

			//aes_send_package(free_pkg[i], &ctx);
			send_package(free_pkg[i]);

			i = (i + 1) % count_free_pkg;
		}
		else set_true(PORTC, PC0);
		
		_delay_ms(100);
	}

	return 0;
}

