#ifndef _SPI_H_
#define _SPI_H_

#define SS PB4
#define MOSI PB5
#define MISO PB6
#define SCK PB7

#define ENC_EMPTY_SPACE 0xee

uint8_t spi_mstr = 0;

inline void spi_set_master()
{
	spi_mstr = 1;
}

inline void spi_set_slave()
{
	spi_mstr = 0;
}

inline uint8_t spi_is_master()
{
	return spi_mstr;
}

inline void spi_init_master()
{
	set_true(DDRB, MOSI);
	set_true(DDRB, SCK);
	set_true(DDRB, SS);

	set_true(SPCR, MSTR); // Device is a master.
	set_true(SPCR, SPR0); // Set SCK = OF / 16.
	set_true(SPCR, SPIE); // Interrupt is enabled.
	set_true(SPCR, SPE);  // SPI is enabled.
}

inline void spi_init_slave()
{
	set_true(DDRB, MISO);

	set_true(SPCR, SPIE);
	set_true(SPCR, SPE);  // SPI is enabled.
}

inline void spi_final()
{
	SPCR = 0;
}

ISR(SPI_STC_vect){}

inline uint8_t sync_byte(uint8_t byte)
{
	cli();

	/* Tricky way of synchronisation */
	if(spi_is_master())
	{
		set_true(DDRB, MOSI);
		set_true(PORTB, MOSI);
		set_false(DDRB, MISO);	

		while(!read_bit(PINB, MISO));
		
		_delay_us(50);
		set_false(PORTB, MOSI);

		spi_init_master();
	}
	else
	{
		spi_sleep();		

		set_true(DDRB, MISO);
		set_true(PORTB, MISO);
		set_false(DDRB, MOSI);

		while(!read_bit(PINB, MOSI));
	
		_delay_us(25);
		set_false(PORTB, MISO);		

		spi_init_slave();
	}
	
	SPDR = byte;
	
	while (!read_bit(SPSR, SPIF)); // Wait for reception complete.
	
	byte = SPDR;
	spi_final();
	sei();

	return byte;
}

inline void send_block(uint8_t * block)
{
	uint8_t i;
	for(i = 0; i < 16; i ++) sync_byte(block[i]);
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

/* Maybe once you will use this funstions */

uint8_t spi_sleep_spcr = 0;

struct
{
	uint8_t arr[100];
	uint8_t cap;
	uint8_t count;
	uint8_t point;
} spi_buf;

inline void spi_sleep()
{
	spi_sleep_spcr = SPCR;
	SPCR = 0;
}

inline void spi_wake_up()
{
	SPCR = spi_sleep_spcr;

	if(spi_mstr)
	{
		set_true(DDRB, MOSI);
		set_true(DDRB, SCK);
		set_true(DDRB, SS);
	}
	else
	{
		set_true(DDRB, MISO); // Output pin of MISO	
	}
}

inline void spi_buf_init()
{
	spi_buf.cap = 100;
	spi_buf.count = 0;
	spi_buf.point = 0;
}

inline uint8_t spi_buf_pull()
{
	if(spi_buf.count == 0) return 0;

	spi_buf.point = (spi_buf.point + 1) % spi_buf.cap;
	spi_buf.count --;

	return spi_buf.arr[(spi_buf.cap - 1 + spi_buf.point) % spi_buf.cap];
}

inline void spi_buf_push(uint8_t byte)
{
	if(spi_buf.count != spi_buf.cap)
	{
		spi_buf.arr[(spi_buf.count + spi_buf.point) % spi_buf.cap] = byte;
		spi_buf.count ++;
	}
}

inline uint8_t spi_buf_is_empty()
{
	return spi_buf.count == 0 ? 1 : 0;
}

#endif // SPI H

