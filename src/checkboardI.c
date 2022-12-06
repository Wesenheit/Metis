#include "assert.h"
#include "utils.h"
#include "stdlib.h"
#include "math.h"
#include "random.h"
#include <stdio.h>
#include "checkboardI.h"

void allocateI(ChessBoardI *b, int s)
{
    b->board=(int_fast8_t**)malloc(s*sizeof(int_fast8_t*));
    b->size=s;
    for (int i=0;i<s;i++)
    {
        b->board[i]=(int_fast8_t*)malloc(s*sizeof(int_fast8_t*));
    }
}

void fillI(ChessBoardI *a, int_fast8_t *tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        a->board[i*a->size]=tab[startx*a->size+up(starty,n)];
        a->board[i*a->size+a->size-1]=tab[startx*a->size+down(starty,n)];
        a->board[i]=tab[up(startx,n)*a->size+starty];
        a->board[(a->size-1)*a->size+i]=tab[down(startx,n)*a->size+starty];
        for (int j=1;j<a->size-1;j++)
        {
            a->board[i*a->size+j]=tab[(startx+i-1)*a->size+j-1];
        }
    }
}

void updateinI(ChessBoardI *a, int_fast8_t *tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        for (int j=1;j<a->size-1;j++)
        {
            tab[(startx+i-1)*a->size+j-1]=a->board[i*a->size+j];
        }
    }
}

void upboundaries(ChessBoardI *a, int_fast8_t *tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        tab[startx*a->size+up(starty,n)]=a->board[i*a->size];
        tab[startx*a->size+down(starty,n)]=a->board[i*a->size+a->size-1];
        tab[up(startx,n)*a->size+starty]=a->board[i];
        tab[down(startx,n)*a->size+starty]=a->board[(a->size-1)*a->size+i];
    }
}

void upboundariesreversed(ChessBoardI *a, int_fast8_t *tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        tab[startx*a->size+ip(starty,n)]=a->board[i*a->size];
        tab[startx*a->size+down(starty,n)]=a->board[i*a->size + a->size-1];
        tab[up(startx,n)*a->size+starty]=a->board[i];
        tab[down(startx,n)*a->size+starty]=a->board[(a->size-1)*a->size+i];
    }
}

void evolveprand(ChessBoardI *chess,unsigned long *seed,int steps,int parity,float T,float B)
{
    for (int i=0;i<steps;i++)
    {
        int a=(randlin(seed)%(chess->size-2))+1;
        int b=(randlin(seed)%(chess->size-2))+1;
        double E=2*(chess->board[a*chess->size+ b])*(B+chess->board[(a+1)*chess->size+b]+chess->board[(a-1)*chess->size+b]+chess->board[a*chess->size+b-1]+chess->board[a*chess->size+1]); //calculating change in energy
        if (E<0) //if energy is smaller just accept...
        {
            chess->board[a*chess->size+b]=-chess->board[a*chess->size+ b];
        }
        else //else accept with given propability
        {
            double c=(double)rand()/(double)RAND_MAX;
            if (c<exp(-E/T))
            {
                chess->board[a*chess->size+ b]=-(chess->board[a*chess->size+ b]);
            }
        }
    }
}

void dealocI(ChessBoardI *a)
{
    free(a->board);
}