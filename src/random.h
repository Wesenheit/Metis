#ifndef RANDOM_H
#define RANDOM_H


#include <math.h>
#include <time.h>
#include <stddef.h>
#define RAND_MAX_ ((1U << 31) - 1)

static inline int randlin(unsigned long *seed)
{
    return (*seed = (*seed * 214013 + 2531011) & RAND_MAX_) >> 16;
}

void set_seeds(unsigned long *seeds,int n)
{
    for (int i = 0; i < n; i++)
    {
        seeds[i]=time(NULL)*i*i;
    }
    
}

uint64_t wyhash64_x; 


uint64_t wyhash64() {
  wyhash64_x += 0x60bee2bee120fc15;
  __uint128_t tmp;
  tmp = (__uint128_t) wyhash64_x * 0xa3b195354a39b70d;
  uint64_t m1 = (tmp >> 64) ^ tmp;
  tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
  uint64_t m2 = (tmp >> 64) ^ tmp;
  return m2/UINT_MAX;
}

#endif
