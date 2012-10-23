/**
* Alice
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "iface.h"
#include "spi.h"
#include "aes.h"

#define BLOCK_SIZE 16

uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };

uint8_t data[] = { 0x01, 0x02, 0x03, 0x04,
                   0x05, 0x06, 0x07, 0x08,
                   0x09, 0x0A, 0x0B, 0x0C,
                   0x0D, 0x0E, 0x0F, 0x00 };

aes128_ctx_t ctx;

void init(void)
{
	set_false(DDRD, PD0); // button
	set_true(PORTD, PD0); // PDO gets extra resistance of 100 kOhm
	set_true(DDRC, PC0); // LED
	set_true(PORTC, PC0); // low voltage
	spi_init_master();

	aes128_init(key, &ctx);
}

int main(void)
{
	int need_to_send = 1;
	uint8_t check = 0;
 
	init();

	aes128_enc(data, &ctx);
 
	while (1)
	{
		check = read_bit(PIND, PD0);
		
		if (check == 0 && need_to_send)
		{
			set_false(PORTC, PC0);
			need_to_send = 0;

			for (int i = 0; i < BLOCK_SIZE; i ++) spi_sync(data[i]);
		}
		else set_true(PORTC, PC0);
		
		//spi_sync(check);
		_delay_ms(50);
	}

	return 0;
}

