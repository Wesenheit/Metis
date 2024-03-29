#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "random.h"
#include "utils.h"

#define RANDMAX 10000000

typedef struct
{
    PyObject_HEAD;
    int n;
    int_fast8_t *tab;
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
    int n;
    int fill;
    static char *kwlist[] = {"n",  "fill",NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii", kwlist,&n,&fill)) {return -1;}
    self->tab=(int_fast8_t *)malloc(n*n*sizeof(int_fast8_t)); //allocating memory for table
    self->n=n;
    for (int i=0;i<n;i++) //setting inital table state
    {   
        for (int j=0;j<n;j++)   
        {
            if (fill)
            {
                self->tab[i*n+j]=1;
            }
            else
            {
                if (rand() % 2)
                {
                    self->tab[i*n+j]=1;
                }
                else
                {
                    self->tab[i*n+j]=-1;
                }
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
            wyn+=self->tab[i*n+j];
        }
    }
    double odp=wyn/(n*n);
    return PyFloat_FromDouble(odp);
}
static PyObject *
board_energy(board *self,PyObject *Py_UNUSED(ignored))
{
    double energy=0;
    int n=self->n;
    for (int a=0;a<n;a++)
    {
        for (int b=0;b<n;b++)
        {
            energy+=self->tab[a*n+b]*(self->tab[a*n+down(b,n)]+self->tab[down(a,n)*n+b]+self->tab[up(a,n)*n+b]+self->tab[a*n+up(b,n)]);
        }
    }
    return PyFloat_FromDouble(energy/2);
}

void eval(board *self,float T,float B)
{
    int a,b;
    a=rand()%(self->n);
    b=rand()%(self->n);
    int n=self->n;
    double E=2*self->tab[a*n+b]*(B+self->tab[a*n+down(b,n)]+self->tab[down(a,n)*n+b]+self->tab[up(a,n)*n+b]+self->tab[a*n+up(b,n)]); //calculating change in energy
    if (E<0) //if energy is smaller just accept...
    {
        self->tab[a*n+b]=-self->tab[a*n+b];
    }
    else //else accept with given propability
    {
        double c=(double)rand()/(double)RAND_MAX;
        if (c<exp(-E/T))
        {
            self->tab[a*n+b]=-(self->tab[a*n+b]);
        }
    }

}

void eval_heat_bath(board *self,float T,float B)
{
    int a,b;
    a=rand()%(self->n);
    b=rand()%(self->n);
    int n=self->n;
    double E=2*(B+self->tab[a*n+down(b,n)]+self->tab[down(a,n)*n+b]+self->tab[up(a,n)*n+b]+self->tab[a*n+up(b,n)]); //calculating change in energy
    double c=(double)rand()/(double)RAND_MAX;
    if (c<1/(1+exp(-E/T)))
    {
        self->tab[a*n+b]=1;
    }
    else
    {
        self->tab[a*n+b]=-1;
    }
}

void eval_heat_bath_fast(board *self,float T,float B,float mul_factor,float *precomp)
{
    int a,b;
    a=rand()%(self->n);
    b=rand()%(self->n);
    int n=self->n;
    int_fast8_t E=(self->tab[a*n+down(b,n)]+self->tab[down(a,n)*n+b]+self->tab[up(a,n)*n+b]+self->tab[a*n+up(b,n)]); //calculating change in energy
    double c=(double)rand()/(double)RAND_MAX;
    if (1/(c+0.00001)-1>precomp[E+4]*mul_factor)
    {
        self->tab[a*n+b]=1;
    }
    else
    {
        self->tab[a*n+b]=-1;
    }
}


void eval_fast(board *self,float T,float B,float mul_factor,float* precomp)
{
    int a,b;
    a=rand()%(self->n);
    b=rand()%(self->n);
    int n=self->n;
    int_fast8_t E=self->tab[a*n+b]*(self->tab[a*n+down(b,n)]+self->tab[down(a,n)*n+b]+self->tab[up(a,n)*n+b]+self->tab[a*n+up(b,n)]); //calculating change in energy
    if (E<0) //if energy is smaller just accept...
    {
        self->tab[a*n+b]=-self->tab[a*n+b];
    }
    else //else accept with given propability
    {
        //double c=(double)wyhash64()/(double)UINT64_MAX;
        double c=(double) rand()/(double)RAND_MAX;
        if (c<precomp[E]*mul_factor)
        {
            self->tab[a*n+b]=-(self->tab[a*n+b]);
        }
    }

}

void eval_stat(board *self,float T,float B)
{
    int a,b;
    a=rand()%(self->n-2)+1;
    b=rand()%(self->n-2)+1;
    int n=self->n;
    double E=2*self->tab[a*n+b]*(B+self->tab[a*n+b-1]+self->tab[(a-1)*n+b]+self->tab[(a+1)*n+b]+self->tab[a*n+b+1]); //calculating change in energy
    if (E<0) //if energy is smaller just accept...
    {
        self->tab[a*n+b]=-self->tab[a*n+b];
    }
    else //else accept with given propability
    {
        double c=(double)rand()/(double)RAND_MAX;
        if (c<exp(-E/T))
        {
            self->tab[a*n+b]=-(self->tab[a*n+b]);
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
board_heath_bath_periodic(board *self, PyObject *args) //single thread implementation of MC algo
{
    long number_of_steps;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"lff",&number_of_steps,&T,&B)){return NULL;}
    for (long i=0;i<number_of_steps;i++)
    {
        eval_heat_bath(self,T,B);   
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
board_heath_bath_periodic_fast(board *self, PyObject *args) //single thread implementation of MC algo
{
    long number_of_steps;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"lff",&number_of_steps,&T,&B)){return NULL;}
    float mul_factor=expf(-2*B/T);
    float* precomp=malloc(9*sizeof(float));
    for (int i=0;i<9;i++)
    {
        precomp[i]=expf(-((float) i-4.)*2/T);
    }
    for (long i=0;i<number_of_steps;i++)
    {
        eval_heat_bath_fast(self,T,B,mul_factor,precomp);   
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *
board_MC_periodic_fast(board *self, PyObject *args) //single thread implementation of MC algo
{
    long number_of_steps;
    float T;
    float B;
    if (!PyArg_ParseTuple(args,"lff",&number_of_steps,&T,&B)){return NULL;}
    float mul_factor=expf(-2*B/T);
    float* precomp=malloc(5*sizeof(float));
    for (int i=0;i<5;i++)
    {
        precomp[i]=expf(-((float)i)*2/T);
    }
    for (long i=0;i<number_of_steps;i++)
    {
        eval_fast(self,T,B,mul_factor,precomp);   
    }
    free(precomp);
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *
board_MC_static(board *self, PyObject *args) //single thread implementation of MC algo
{
    long number_of_steps;
    float T;
    float B;
    int n=self->n;
    if (!PyArg_ParseTuple(args,"lff",&number_of_steps,&T,&B)){return NULL;}
    for (int i = 0; i < self->n; i++)
    {
        self->tab[i*n+n-1]=1;
        self->tab[i*n+0]=1;
        self->tab[n*(n-1)+i]=1;
        self->tab[i]=1;
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
            if (self->tab[i*n+j]==1)
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
            if(self->tab[i*n+j]==-1)
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
    {"energy", (PyCFunction) board_energy,METH_NOARGS,
    "calculate energy"},
    {"evolve_per",(PyCFunction) board_MC_periodic,METH_VARARGS,
    "evolve the board with Metropolis-Hastings algorithm(periodic boundary conditions)"},
    {"evolve_heath_bath",(PyCFunction) board_heath_bath_periodic,METH_VARARGS,
    "evolve the board with heath bath MC algorithm (periodic boundary conditions)"},
    {"evolve_heath_bath_fast",(PyCFunction) board_heath_bath_periodic_fast,METH_VARARGS,
    "DOES NOT WORK WITH MAGNETIC FIELD, evolve the board with heath bath MC algorithm (periodic boundary conditions)"},
    {"evolve_sta",(PyCFunction) board_MC_static,METH_VARARGS,
    "evolve the board with Metropolis-Hastings algorithm(static boundary conditions)"},
    {"show",(PyCFunction) board_show, METH_NOARGS,
    "print the board"
    },
    {"flatten",(PyCFunction) board_flatten,METH_NOARGS,
    "flatten board"},
    {"evolve_per_fast",(PyCFunction) board_MC_periodic_fast,METH_VARARGS,
    "DOES NOT WORK WITH MAGNETIC FIELD, evolve the board with fast Metropolis-Hastings algorithm (periodic boundary conditions)"},
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