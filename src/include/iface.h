#ifndef _IFACE_H_
#define _IFACE_H_

#define RAND_MAX 0x7fff
#define set_true(in, pos) in |= (1 << pos)
#define set_false(in, pos) in &= ~(1 << pos)
#define read_bit(in, pos) ((in & (1 << pos)) ? 1 : 0)
#define frand(x)((uint64_t)(rand() * 1.0 * (x) / RAND_MAX))
#define SQ(x) (x * x)

inline void srand_init()
{
	uint8_t i = 0;
	uint16_t srand_value = 0;

	ADMUX = (1 << REFS1) | (1 << REFS0); // Use internal voltage.
	ADMUX |= 1 << ADLAR; // Left adjusted result.
	ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // Read signal from 7 pin of port A.

	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0); // Frequency division is 64;
	ADCSRA |= (1 << ADEN) | (1 << ADSC); // ADC is enabled and conversation is started.

	while(1)
	{
		if(read_bit(ADCSRA, ADIF))
		{
			srand_value |= read_bit(ADCL, 6);
			ADCH;
			srand_value <<= 1;
			i ++;

			if(i == 16) break;
			set_false(ADCSRA, ADIF);
			set_true(ADCSRA, ADSC);
		}
	}

	ADCSRA = 0;
	ADMUX = 0;

	srand(srand_value);
}

#endif // IFACE H
