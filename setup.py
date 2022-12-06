from setuptools import setup,Extension

parallel=["-fopenmp","-std=c11","-march=native"]
parallel_links=['-lgomp']


module_ising2D = Extension('Ising', sources = ['src/ising.c',])
para_ising2D = Extension('ParaIsing', sources = ['src/Para_ising.c',"src/checkboardI.c"],extra_compile_args=parallel,extra_link_args=parallel_links)
setup (name = 'Metis',
       version = '1.0',
       description = 'MC statistical mechanic simulator',
       ext_modules = [module_ising2D,para_ising2D],
       platforms=['POSIX',]
       )
