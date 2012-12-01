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

uint32_t timer0_counter = 0;
uint32_t timer0_counter_buf = 0;

ISR(TIMER0_OVF_vect)
{
	timer0_counter ++;
}

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
	uint8_t k, count_free_pkg = 4;
	uint8_t key[16], pkg_buf[48];
	uint8_t free_pkg[][40] =
	{
		"Hello, everybody! I'm SPI secure!",
		"After each click send 1 msg."     ,
		"This is the third msg from Alice.",
		"We need to detect time of trans."
	};	
	
	uint8_t test_pkg[] =
	{
		"qbyluiemkgopsplfiresdhjndfzxunop"
	};
	
	memset(key, 0, 16);
	memset(pkg_buf, 0, 48);

	uint16_t i = 0;

	init();

	/* Getting private key by Diffie-Hellman algorithm with tricky method */
	while(1)
	{
		if(!read_bit(PIND, 7))
		{	
			set_false(PORTC, 0);
			difhel_private_key(key, (uint16_t) 128);
			aes128_init(key, &ctx);

			set_true(PORTC, 0);
			_delay_ms(100);	
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
	
			//sprintf(pkg_buf, "<%d><%lu>", i, get_part_key(key, i));
			//send_package(pkg_buf);
			aes_send_package(free_pkg[i], &ctx);

			i = (i + 1) % count_free_pkg;
		}
		else set_true(PORTC, 0);
		
		_delay_ms(100);
	}
	
	/* Time test of transmission */
	uint8_t test_type_ind = 1;
	uint16_t count_test_pkg = 500;
	
	double timer0_counter_ms_in_step = (255 << 5) * 0.001; // == 255 * 256 / (8 * 10 ^ 6) sec		

	while(0)
	{
		if (!read_bit(PIND, 7))
		{
			set_false(PORTC, 0);
		
			k = 0;
			timer0_counter_buf = 0;

			while(k < 5)			
			{
				/* Timer 0 init */
				TCCR0 = 1 << CS02; // Frequency division is 256.
				TIMSK |= 0 << TOIE0; // Overflow interrupt is disabled.
	
				timer0_counter = 0;
			
				/* Synchronization */
				while(!read_bit(TIFR, TOV0));
			
				set_false(TIFR, TOV0);
				TIMSK |= 1 << TOIE0; // Overflow interrupt is enabled.
			
				/* Begin counter */

				for(i = 0; i < count_test_pkg; i ++)
				{
					test_pkg[0] = (uint8_t)(65 + (i % 26));
				
					if(test_type_ind) aes_send_package_invis(test_pkg, &ctx);
					else send_package_invis(test_pkg);
				}

				/* Stop counter */
				TIMSK |= 0 << TOIE0;
				
				timer0_counter_buf += timer0_counter;
				k ++;
			}

			sprintf(pkg_buf, "%d_%s_%3.1fms",
							  count_test_pkg * strlen((char *)test_pkg),
							  test_type_ind ? "A" : "O",
							  timer0_counter_buf * timer0_counter_ms_in_step / 5);
			aes_send_package(pkg_buf, &ctx);

			if(count_test_pkg >= 500)
			{
				test_type_ind = (test_type_ind + 1) % 2;
				count_test_pkg = 0;
			}

			count_test_pkg += 25;
		}
		else set_true(PORTC, 0);

		_delay_ms(100);
	}

	return 0;
}

