#ifndef _DIFHEL_H_
#define _DIFHEL_H_

typedef uint32_t auth_t;
const uint8_t count_byte_key = sizeof(auth_t);
const auth_t PUBLIC_PRIME_MODULO = 38782727; // safe prime number
const auth_t PUBLIC_PRIME_BASE = 5; // primitive root

auth_t pow_modulo(auth_t _base, auth_t pow, auth_t modulo)
{
	_base %= modulo;

	uint64_t res = 1;
	uint64_t base = (uint64_t) _base;
	
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
	return (auth_t)frand((uint64_t)(PUBLIC_PRIME_MODULO - 1));
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
	if(num_byte >= count_byte_key) {*out = 0; return;}

	number >>= num_byte << 3;
	*out = (uint8_t) (number & 255);
}

void difhel_private_key(uint8_t * key, uint16_t key_length)
{
	uint8_t i, k, buf;
	auth_t pub_key_own = 0, pr_key = 0, pub_key = 0, pr_inter_key = 0;

	key_length >>= 3;
	key_length /= count_byte_key;

	for(i = 0; i < key_length; i ++)
	{
		pub_key = 0;
		pr_inter_key = private_inter_key();
		pub_key_own = public_key(pr_inter_key);

		for(k = 0; k < count_byte_key; k ++)
		{
			buf = 0;
			get_byte_of_number(&buf, pub_key_own, count_byte_key - 1 - k);
			buf = sync_byte(buf);
			pub_key |= buf;
			if(k != count_byte_key - 1) pub_key <<= 1 << 3;
		}

		pr_key = private_key(pr_inter_key, pub_key);

		for(k = 0; k < count_byte_key; k ++)
		{
			//if (i == key_length) get_byte_of_number(key + k, pr_key, count_byte_key - 1 - k);
			get_byte_of_number(key + (i * count_byte_key + k), pr_key, count_byte_key - 1 - k);
		}
	}
}

auth_t get_part_key(uint8_t * key, uint8_t number)
{
	uint8_t i;
	auth_t res = 0;
	
	for(i = 0; i < count_byte_key; i ++)
	{
		res |= key[number * count_byte_key + i];
		if(i != count_byte_key - 1) res <<= 1 << 3;
	}

	return res;
}

#endif // DIFHEL H
