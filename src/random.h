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
#endif
