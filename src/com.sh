gcc -fPIC -Wall -c -I/usr/include/python3.8 ising.c
gcc -fPIC -Wall -shared ising.o -o ising.so
