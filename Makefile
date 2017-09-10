
default : simdsort.c 
	gcc -msse3 -o simdsort simdsort.c 