#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "random.h"
#include "utils.h"
#include "checkboardI.h"
typedef struct
{
    PyObject_HEAD;
    int n;
    char **tab;
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
    self->tab=(char **)malloc(n*sizeof(char*)); //allocating memory for table
    for (int i=0;i<n;i++)
    {
        self->tab[i]=(char *)malloc(n* sizeof(char));
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
board_mean_char(board *self, PyObject *Py_UNUSED(ignored))
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

static PyObject *
board_MC_para(board *self, PyObject *args)//parallel implementation of MC algo
{
    int number_of_steps;
    int cores;
    int par;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"iffii",&number_of_steps,&T,&B,&cores,&par)){return NULL;}
    omp_set_num_threads(cores);
    #pragma omp parallel for
    for (int num=0;num<par*par;num++)
    {
        unsigned long seed=omp_get_thread_num()*2131423*num;
        ChessBoardI Chess;
        allocateI(&Chess,self->n/par+2);
        fillI(&Chess,self->tab,self->n,num,par);
        int iters_per_square=(self->n/par)*(self->n/par)/4;
        for (int j=0;j<number_of_steps/(iters_per_square*par*par);j++)
        {
            evolveprand(&Chess,&seed,iters_per_square,0,T,B);
            //updateinI(&Chess,self->tab,self->n,num,par);
            //upboundaries(&Chess,self->tab,self->n,num,par);
            evolveprand(&Chess,&seed,iters_per_square,1,T,B);
            //updateinI(&Chess,self->tab,self->n,num,par);
            //upboundaries(&Chess,self->tab,self->n,num,par);
        }
        dealocI(&Chess);
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
    {"mean", (PyCFunction) board_mean_char, METH_NOARGS,
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