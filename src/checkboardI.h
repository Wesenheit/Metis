#include <stdint.h>
typedef struct ChessBoardI
{
    int_fast8_t *board;
    int size;

} ChessBoardI;
void allocateI(ChessBoardI *b, int s);
void fillI (ChessBoardI *b, int_fast8_t *tab, int n, int num, int part);
void updateinI(ChessBoardI *a, int_fast8_t *tab, int n, int num, int p);
void evolveprand(ChessBoardI *chess,unsigned long *seed,int steps,int parity,float T,float B);
void upboundaries(ChessBoardI *a, int_fast8_t *tab, int n, int num, int p);
void dealocI(ChessBoardI *a);