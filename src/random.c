#include <stddef.h>
#include <time.h>
#define RAND_MAX ((1U << 31) - 1)

/*
int randlin(unsigned long *seed)
{
    return (*seed = (*seed * 214013 + 2531011) & RAND_MAX) >> 16;
}*/

void set_seeds(unsigned long *seeds,int n)
{
    for (int i = 0; i < n; i++)
    {
        seeds[i]=time(NULL)*i*i;
    }
    
}
