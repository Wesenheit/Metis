typedef struct ChessBoardI
{
    char **board;
    int size;

} ChessBoardI;
void allocateI(ChessBoardI *b, int s);
void fillI (ChessBoardI *b, char **tab, int n, int num, int part);
void updateinI(ChessBoardI *a, char **tab, int n, int num, int p);
void evolveprand(ChessBoardI *chess,unsigned long *seed,int steps,int parity,float T,float B);
void upboundaries(ChessBoardI *a, char **tab, int n, int num, int p);
void dealocI(ChessBoardI *a);