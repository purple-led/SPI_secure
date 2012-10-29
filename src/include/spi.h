#ifndef _SPI_H_
#define _SPI_H_

#define SS PB4
#define MOSI PB5
#define MISO PB6
#define SCK PB7

#define ENC_EMPTY_SPACE 0xee

inline void spi_init_master(void)
{
	set_true(DDRB, MOSI);
	set_true(DDRB, SCK);
	set_true(DDRB, SS);
	
	set_true(SPCR, MSTR);
	set_true(SPCR, SPR0); // set SCK = OF / 16
	set_true(SPCR, SPE);  // spi enable
}

inline void spi_init_slave(void)
{
	set_true(DDRB, PB6); // output pin of MISO
	set_true(SPCR, SPE); // spi enable
}

inline uint8_t sync_byte(uint8_t byte)
{
	SPDR = byte;
	while (!read_bit(SPSR, SPIF)); // wait for reception complete
	return SPDR;
}

inline void send_block(uint8_t * block)
{
	uint8_t i;
	for(i = 0; i < 16; i ++) sync_byte(block[i]);
	_delay_ms(5);
}

inline void receive_block(uint8_t * block)
{
	uint8_t i;
	for(i = 0; i < 16; i ++) block[i] = sync_byte(0);
}

inline void send_package(uint8_t * pkg)
{
	void * ctx;
	_send_package(pkg, 0, ctx);
}

inline void aes_send_package(uint8_t * pkg, void * ctx)
{
	_send_package(pkg, 1, ctx);
}

inline void _send_package(uint8_t * pkg, uint8_t check_aes, void * ctx)
{
	int i, length = strlen((char *) pkg);
	uint8_t k, aes_block[16];

	for(i = 0; i < length >> 4; i ++)
	{
		for(k = 0; k < 16; k ++) aes_block[k] = pkg[(i << 4) + k];
		
		if(check_aes) aes128_enc(aes_block, ctx);
		send_block(aes_block);
	}

	if(i << 4 != length)
	{
		for(k = 0; k < length - (i << 4); k++) aes_block[k] = pkg[(i << 4) + k];
		for(; k < 16; k ++) aes_block[k] = ENC_EMPTY_SPACE;
	
		if(check_aes) aes128_enc(aes_block, ctx);
		send_block(aes_block);
	}
	
	for(k = 0; k < 16; k ++) aes_block[k] = 0xff;
	send_block(aes_block);
}

inline void receive_package(uint8_t * pkg)
{
	void * ctx;
	_receive_package(pkg, 0, ctx);
}

inline void aes_receive_package(uint8_t * pkg, void * ctx)
{
	_receive_package(pkg, 1, ctx);
}

inline void _receive_package(uint8_t * pkg, uint8_t check_aes, void * ctx)
{
	int i = 0;
	uint8_t k, check_end, aes_block[16];
	
	while(1)
	{
		receive_block(aes_block);

		check_end = 1;
		for(k = 0; k < 16; k ++) if(aes_block[k] != 0xff) {check_end = 0; break;}
		
		if(check_end && i)
		{
			i --;

			for(k = 0; k < 16; k ++)
			{
				if(pkg[(i << 4) + k] == ENC_EMPTY_SPACE) break;
			}

			pkg[(i << 4) + k] = '\0';

			break;
		}
		else if(check_end && !i) pkg[0] = '\0';

		if(check_aes) aes128_dec(aes_block, ctx);
		for(k = 0; k < 16; k ++) pkg[(i << 4) + k] = aes_block[k];

		i ++;
	}
}

#endif // SPI H

