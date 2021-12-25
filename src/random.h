#include <math.h>
static inline int randlin(unsigned long *seed)
{
    return (*seed = (*seed * 214013 + 2531011) & RAND_MAX) >> 16;
}

void set_seeds(unsigned long *seeds,int n);
