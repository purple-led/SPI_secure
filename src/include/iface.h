#ifndef _IFACE_H_
#define _IFACE_H_

#define RAND_MAX 0x7fff
#define set_true(in, pos) in |= (1 << pos)
#define set_false(in, pos) in &= ~(1 << pos)
#define read_bit(in, pos) ((in & (1 << pos)) ? 1 : 0)
#define frand(x)((uint64_t)(rand() * 1.0 * (x) / RAND_MAX))
#define SQ(x) (x * x)

#endif // IFACE H
