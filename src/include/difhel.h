#ifndef _DIFHEL_H_
#define _DIFHEL_H_

typedef uint32_t auth_t;
#define PUBLIC_PRIME_MODULO (auth_t) 30539 // 2447// 33623 // safe prime number
#define PUBLIC_PRIME_BASE   (auth_t) 6 // 5 // 5 // primitive root

auth_t pow_modulo(uint32_t base, uint32_t pow, uint32_t modulo)
{
	uint32_t res = 1;
	
	base %= modulo;
	
	while(pow)
	{
		if(read_bit(pow, 0))
		{
			pow --;
			res = (res * base) % modulo;
		}
		else
		{
			pow >>= 1;
			base = (base * base) % modulo;
		}
	}

	return (auth_t) res;
}

auth_t private_inter_key()
{
	return (auth_t)frand(PUBLIC_PRIME_MODULO - 1);
}

auth_t public_key(auth_t private_inter_key)
{
	return pow_modulo(PUBLIC_PRIME_BASE, private_inter_key, PUBLIC_PRIME_MODULO);
}

auth_t private_key(auth_t private_inter_key, auth_t public_key)
{
	return pow_modulo(public_key, private_inter_key, PUBLIC_PRIME_MODULO);
}

void get_byte_of_number(uint8_t * out, auth_t number, uint8_t num_byte)
{
	if(num_byte >= sizeof(auth_t))
	{
		*out = 0;
		return;
	}

	number >>= num_byte << 3;
	*out = (uint8_t) (number & 255);
}

void difhel_private_key(uint8_t * key, uint16_t key_length)
{
	uint8_t i, k, buf = 0;
	auth_t pub_key_own = 0, pr_key = 0, pub_key = 0, pr_inter_key = 0;

	key_length >>= 5;

	for(i = 0; i < key_length; i ++)
	{
		pub_key = 0;
		pr_inter_key = private_inter_key();
		pub_key_own = public_key(pr_inter_key);

		for(k = 0; k < 4; k ++)
		{
			get_byte_of_number(&buf, pub_key_own, 3 - k);
			buf = sync_byte(buf);
			pub_key |= buf;
			if(k != 3) pub_key <<= 8;
		}
	
		pr_key = private_key(pr_inter_key, pub_key);

		for(k = 0; k < 4; k ++)
		{
			get_byte_of_number(key + (i << 2) + k, pr_key, 3 - k);
		}
	}
}

uint32_t get_part_key(uint8_t * key, uint8_t number)
{
	uint8_t i;
	uint32_t res = 0;
	
	for(i = 0; i < 4; i ++)
	{
		res |= key[(number << 2) + i];
		if(i != 3) res <<= 1 << 3;
	}

	return res;
}

#endif // DIFHEL H
