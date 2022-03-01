#include "assert.h"
#include "utils.h"
#include "stdlib.h"
#include "math.h"
#include "random.h"
#include <stdio.h>
typedef struct ChessBoardI
{
    char **board;
    int size;

} ChessBoardI;

void allocateI(ChessBoardI *b, int s)
{
    b->board=(char**)malloc(s*sizeof(char*));
    b->size=s;
    for (int i=0;i<s;i++)
    {
        b->board[i]=(char*)malloc(s*sizeof(char*));
    }
}

void fillI(ChessBoardI *a, char **tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        a->board[i][0]=tab[startx][up(starty,n)];
        a->board[i][a->size-1]=tab[startx][down(starty,n)];
        a->board[0][i]=tab[up(startx,n)][starty];
        a->board[a->size-1][i]=tab[down(startx,n)][starty];
        for (int j=1;j<a->size-1;j++)
        {
            a->board[i][j]=tab[startx+i-1][starty+j-1];
        }
    }
}

void updateinI(ChessBoardI *a, char **tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        for (int j=1;j<a->size-1;j++)
        {
            tab[startx+i-1][starty+j-1]=a->board[i][j];
        }
    }
}

void upboundaries(ChessBoardI *a, char **tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        tab[startx][up(starty,n)]=a->board[i][0];
        tab[startx][down(starty,n)]=a->board[i][a->size-1];
        tab[up(startx,n)][starty]=a->board[0][i];
        tab[down(startx,n)][starty]=a->board[a->size-1][i];
    }
}

void upboundariesreversed(ChessBoardI *a, char **tab, int n, int num, int p)
{
    int starty=(num/p)*n/p;
    int startx=(num%p)*n/p;
    for (int i=1;i<a->size-1;i++)
    {
        tab[startx][up(starty,n)]=a->board[i][0];
        tab[startx][down(starty,n)]=a->board[i][a->size-1];
        tab[up(startx,n)][starty]=a->board[0][i];
        tab[down(startx,n)][starty]=a->board[a->size-1][i];
    }
}

void evolveprand(ChessBoardI *chess,unsigned long *seed,int steps,int parity,float T,float B)
{
    for (int i=0;i<steps;i++)
    {
        int a=(randlin(seed)%(chess->size-2))+1;
        int b=(randlin(seed)%(chess->size-2))+1;
        double E=2*(chess->board[a][b])*(B+chess->board[a+1][b]+chess->board[a-1][b]+chess->board[a][b-1]+chess->board[a][b+1]); //calculating change in energy
        if (E<0) //if energy is smaller just accept...
        {
            chess->board[a][b]=-chess->board[a][b];
        }
        else //else accept with given propability
        {
            double c=(double)rand()/(double)RAND_MAX;
            if (c<exp(-E/T))
            {
                chess->board[a][b]=-(chess->board[a][b]);
            }
        }
    }
}

void dealocI(ChessBoardI *a)
{
    for (int i=0;i<a->size;i++)
    {
        free(a->board[i]);
    }
    free(a->board);
}