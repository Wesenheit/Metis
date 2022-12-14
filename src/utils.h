#ifndef UTILS_H
#define UTILS_H
#include <limits.h>
#include <stdint.h>
static inline int up (int x,int n) //upper spin with periodic boundary conditions
{
    if (x==n-1)
    {
        return 0;
    }
    else
    {
        return x+1;
    }
}

static inline int down (int x,int n) //lower spin with periodic boundary conditions
{
    if (x==0)
    {
        return n-1;
    }
    else
    {
        return x-1;
    }
}
#endif