from distutils.core import setup, Extension

parallel=["-fopenmp","-std=c11"]
module_ising2D = Extension('Ising', sources = ['src/ising.c'],extra_compile_args=parallel,extra_link_args=['-lgomp'])

setup (name = 'Metis',
       version = '1.0',
       description = 'MC statistical mechanic simulator',
       ext_modules = [module_ising2D])
