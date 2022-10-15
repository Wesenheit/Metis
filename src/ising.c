#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "random.h"
#include "utils.h"

#define RANDMAX 10000000

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
    static char *kwlist[] = {"n",  "fill",NULL};
    int fill_with_ones;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii", kwlist,&self->n,&fill_with_ones)) {return -1;}
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
            int c;
            if (fill_with_ones)
            {
                c=rand()&2;
            }
            else
            {
                c=1;
            }
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
void eval(board *self,float T,float B)
{
    int a,b;
    a=rand()%(self->n);
    b=rand()%(self->n);
    double E=2*self->tab[a][b]*(B+self->tab[a][down(b,self->n)]+self->tab[down(a,self->n)][b]+self->tab[up(a,self->n)][b]+self->tab[a][up(b,self->n)]); //calculating change in energy
    if (E<0) //if energy is smaller just accept...
    {
        self->tab[a][b]=-self->tab[a][b];
    }
    else //else accept with given propability
    {
        double c=(double)rand()/(double)RAND_MAX;
        if (c<exp(-E/T))
        {
            self->tab[a][b]=-(self->tab[a][b]);
        }
    }

}

void eval_stat(board *self,float T,float B)
{
    int a,b;
    a=rand()%(self->n-2)+1;
    b=rand()%(self->n-2)+1;
    double E=2*self->tab[a][b]*(B+self->tab[a][b-1]+self->tab[a-1][b]+self->tab[a+1][b]+self->tab[a][b+1]); //calculating change in energy
    if (E<0) //if energy is smaller just accept...
    {
        self->tab[a][b]=-self->tab[a][b];
    }
    else //else accept with given propability
    {
        double c=(double)rand()/(double)RAND_MAX;
        if (c<exp(-E/T))
        {
            self->tab[a][b]=-(self->tab[a][b]);
        }
    }
}

static PyObject *
board_MC_periodic(board *self, PyObject *args) //single thread implementation of MC algo
{
    long number_of_steps;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"lff",&number_of_steps,&T,&B)){return NULL;}
    for (long i=0;i<number_of_steps;i++)
    {
        eval(self,T,B);   
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
board_MC_static(board *self, PyObject *args) //single thread implementation of MC algo
{
    long number_of_steps;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"lff",&number_of_steps,&T,&B)){return NULL;}
    for (int i = 0; i < self->n; i++)
    {
        self->tab[i][self->n-1]=1;
        self->tab[i][0]=1;
        self->tab[self->n-1][i]=1;
        self->tab[0][i]=1;
    }
    
    for (long i=0;i<number_of_steps;i++)
    {
        eval_stat(self,T,B);   
    }
    Py_INCREF(Py_None);
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
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
board_flatten(board *self)
{
    int n=self->n;
    char tab[n*n+1];
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            if(self->tab[i][j]==-1)
            {
                tab[i*n+j]='0';
            }
            else
            {
                tab[i*n+j]='1';
            }
        }
    }
    tab[n*n]='\0';
    const char * wyn=tab;
    PyObject *str=PyUnicode_FromString(wyn);
    return str;
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
    {"evolve_per",(PyCFunction) board_MC_periodic,METH_VARARGS,
    "evolve the board with Metropolis-Hastings algorithm(periodic boundary conditions)"},
    {"evolve_sta",(PyCFunction) board_MC_static,METH_VARARGS,
    "evolve the board with Metropolis-Hastings algorithm(static boundary conditions)"},
    {"show",(PyCFunction) board_show, METH_NOARGS,
    "print the board"
    },
    {"flatten",(PyCFunction) board_flatten,METH_NOARGS,
    "flatten board"},
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
    .m_name = "Ising",
    .m_doc = "Ising 2D board MC simulator",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_Ising(void)
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