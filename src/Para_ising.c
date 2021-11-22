#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "random.h"

typedef struct
{
    PyObject_HEAD;
    int n;
    int **tab;
} board;

static PyObject *
board_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    board *self;
    self = (board *) type->tp_alloc(type, 0);
    if (self != NULL) 
    {
        self->tab=NULL;
        self->n=0;
    }
    return (PyObject *) self;
}

static int
board_init(board *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"n",  NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist,&self->n)) {return -1;}
    int n=self->n;
    self->tab=(int **)malloc(n*sizeof(int*)); //allocating memory for table
    for (int i=0;i<n;i++)
    {
        self->tab[i]=(int *)malloc(n* sizeof(int));
    }
    for (int i=0;i<n;i++) //setting inital table state
    {   
        for (int j=0;j<n;j++)   
        {
            int c=rand()%2;
            if (c==1)
            {
                self->tab[i][j]=1;
            }
            else
            {
                self->tab[i][j]=-1;
            }
        }
    }
    return 0;
}

static PyObject *
board_mean(board *self, PyObject *Py_UNUSED(ignored))
{
    double wyn=0;
    int n=self->n;
    for (int i=0;i<n;i++) //calulating mean magnetisation
    {
        for (int j=0;j<n;j++)
        {
            wyn+=self->tab[i][j];
        }
    }
    double odp=wyn/(n*n);
    return PyFloat_FromDouble(odp);
}
int up (int x,int n) //upper spin with periodic boundary conditions
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
int down (int x,int n) //lower spin with periodic boundary conditions
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


void eval_para(board *self,float T,float B,int num,unsigned long *seed)
{
    double c;
    int a=randlin(seed)%self->n;
    int b=randlin(seed)%self->n;
    //a=cords[2*num]; //assigning cords to change
    //b=cords[2*num+1];
    double E=2*self->tab[a][b]*2*(B+self->tab[a][down(b,self->n)]+self->tab[down(a,self->n)][b]+self->tab[up(a,self->n)][b]+self->tab[a][up(b,self->n)]); //same as in single thread
    if (E<0)
    {
        self->tab[a][b]=-self->tab[a][b];
        
    }
    else
    {
        //c=0;
        c=rand()%10000/10000;
        if (c>exp(-E/T))
        {
            self->tab[a][b]=-(self->tab[a][b]);
            
        }
    }

}
void gen(int *cords,unsigned long *seeds,int cores,int n) //generate table with coordinates for each thread with one master thread
{
    int a,b;
    int flaga=1;
    for (int i=0;i<cores;i++)
    {
        while (flaga)
        {
            a=(int) randlin(&seeds[0])%n;
            b=(int) randlin(&seeds[0])%n;
            flaga=0;
            for (int j=0;j<i;j++)
            {
                if (abs(cords[2*j]-a)<=1 || abs(cords[2*j]-a)==n-1 ||abs(cords[2*j+1]-b)==n-1 ||abs(cords[2*j+1]-b)<=1)//checking if change at given coordinates does not overlap with other threads work
                {
                    flaga=1;
                }
            }
            
        }
        flaga=1;
        cords[2*i]=a;
        cords[2*i+1]=b;
    }
}

void gen_each_thread(int *cords,unsigned long *seeds, int *num,int n)//generate cords for each thread (this ttime each thread works for itself)
{
    int a,b;
    a=(int) randlin(&seeds[*num])%n;
    b=(int) randlin(&seeds[*num])%n;
    cords[2*(*num)]=a;
    cords[2*(*num)+1]=b;
}

static PyObject *
board_MC_para(board *self, PyObject *args)//parallel implementation of MC algo
{
    int number_of_steps;
    int cores;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"iffi",&number_of_steps,&T,&B,&cores)){return NULL;}
    omp_set_num_threads(cores);
    int cords[2*cores];
    unsigned long seeds[cores];
    set_seeds(seeds,cores);
    #pragma omp parallel
    {
        int num=omp_get_thread_num();
        unsigned long seed=1231231*num*num;
        for (int i=0;i<number_of_steps/cores;i++)
        {
            
            #pragma omp master
            {
                gen(cords,seeds,cores,self->n);
            }
            #pragma omp barrier
            //gen_each_thread(cords,seeds,&num,self->n);
            eval_para(self,T,B,num,&seed);   
        }
    }
    return Py_None;
}


static PyObject *
board_show(board *self, PyObject *Py_UNUSED(ignored))//print board state
{
    int n=self->n;
    for (int i=0;i<n;i++)
    {
        char tab[n+2];
        for (int j=0;j<n;j++)
        {
            if (self->tab[i][j]==1)
            {
                tab[j]='u';
            }
            else
            {
                tab[j]='d';
            }
        }
        tab[n]='\n';
        tab[n+1]='\0';
        const char* wyn=tab;
        PyObject *str= PyUnicode_FromString(wyn);
        PyObject_Print(str,stdout,1);
        //free(str);
        //Py_DECREF(str);
    }
    return Py_None;
}

static void
board_dealloc(board *self)
{
    int n=self->n;
    for (int i=0;i<n;i++)
    {
        free(self->tab[i]);
    }
    free(self->tab);
    Py_TYPE(self)->tp_free((PyObject *) self);
}
static PyMemberDef board_members[] = {
    {"n", T_INT, offsetof(board, n), 0,
     "size of board"},
    {"board", T_OBJECT, offsetof(board,tab), 0,
     "board"},
    {NULL}  
};

static PyMethodDef board_methods[] = {
    {"mean", (PyCFunction) board_mean, METH_NOARGS,
     "calculate mean value in array"
    },
    {"show",(PyCFunction) board_show, METH_NOARGS,
    "print the board"
    },
    {"evolvep",(PyCFunction) board_MC_para,METH_VARARGS,
    "evolve the board with Metropolis-Hastings algorithm(parallel version)"},
    {NULL}  
};


static PyTypeObject BoardType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "board",
    .tp_doc = "ising board",
    .tp_basicsize = sizeof(board),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = board_new,
    .tp_init = (initproc) board_init,
    .tp_dealloc = (destructor) board_dealloc,
    .tp_members = board_members,
    .tp_methods = board_methods,
};

static PyModuleDef isingmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "ParaIsing",
    .m_doc = "Ising 2D board MC simulator",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_ParaIsing(void)
{
    PyObject *m;
    if (PyType_Ready(&BoardType) < 0)
        return NULL;

    m = PyModule_Create(&isingmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&BoardType);
    if (PyModule_AddObject(m, "Board", (PyObject *) &BoardType) < 0) {
        Py_DECREF(&BoardType);
        Py_DECREF(m);
        return NULL;
    }
    srand(time(NULL));
    return m;
}