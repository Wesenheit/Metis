from distutils.core import setup, Extension

module2D = Extension('ising',
                    sources = ['src/ising.c'])

setup (name = 'Ising',
       version = '1.0',
       description = 'MC ising simulator',
       ext_modules = [module2D])
