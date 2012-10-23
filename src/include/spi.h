#ifndef _SPI_H_
#define _SPI_H_

#define SS PB4
#define MOSI PB5
#define MISO PB6
#define SCK PB7

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

inline uint8_t spi_sync(uint8_t data)
{
	SPDR = data;
	while (!read_bit(SPSR, SPIF)); // wait for reception complete
	return SPDR;
}

#endif // SPI H

