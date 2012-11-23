#ifndef _DIFHEL_H_
#define _DIFHEL_H_

typedef uint32_t auth_t;
#define PUBLIC_PRIME_MODULO (auth_t) 33623 //safe prime number
#define PUBLIC_PRIME_BASE   (uint8_t) 5 // primitive root modulo 33623

inline auth_t pow_modulo(auth_t base, auth_t pow, auth_t modulo)
{
	auth_t res = 1;
	
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
			base = SQ(base) % modulo;
		}
	}

	return res;
}

inline auth_t private_inter_key()
{
	return (auth_t)frand(PUBLIC_PRIME_MODULO - 1);
}

inline auth_t public_key(auth_t private_inter_key)
{
	return pow_modulo(PUBLIC_PRIME_BASE, private_inter_key, PUBLIC_PRIME_MODULO);
}

inline auth_t private_key(auth_t private_inter_key, auth_t public_key)
{
	return pow_modulo(public_key, private_inter_key, PUBLIC_PRIME_MODULO);
}

inline void get_byte_of_number(uint8_t * out, uint32_t number, uint8_t num_byte)
{
	if(num_byte >= 4) return 0;

	number >>= num_byte << 3;
	*out = (uint8_t) (number & 255);
}

inline void difhel_private_key(uint8_t * key, uint16_t key_length)
{
	uint8_t i;
	auth_t pub_key_own, pr_key, pub_key = 0, pr_inter_key;

	uint8_t buf = 0;

	for(i = 0; i < (uint8_t)(key_length >> 4); i ++)
	{
		pr_inter_key = private_inter_key();
		pub_key_own = public_key(pr_inter_key);

		get_byte_of_number(&pub_key, pub_key_own, 1);
		pub_key = sync_byte(pub_key);
		pub_key <<= 8;

		get_byte_of_number(&buf, pub_key_own, 0);
		buf = sync_byte(buf);
		pub_key |= buf;
	
		pr_key = private_key(pr_inter_key, pub_key);

		get_byte_of_number(key + (i << 1), pr_key, 1);
		get_byte_of_number(key + (i << 1) + 1, pr_key, 0);
	}
}

#endif // DIFHEL H
