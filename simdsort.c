#include <stdio.h>
#include <pmmintrin.h>

/* 
	* Laboratorio 1 HPC
	* Obejtivo: Conocer, usar y apreciar las características de paralelismo a nivel de instrucción 
		de un procesador moderno. 
		Implementación de un sistema Mono-Hebreado para ordenamiento de números.
		Merge Bitónico a nivel SIMD para ordenar subsecuencias y luego Merge a nivel de proceso para mezclar
		subsecuencias y obtener la secuencia ordenada. 
	* Profesor Fernando Rannou Fuentes
	* Ayudante Miguel Cárcamo 
*/



/* 
	Bloque de código principal
**Aquí se ejecután las funcionalidades desarrolladas en el orden correspondiente para lograr lo solicitado.
*/
int main(int argc, char **argv)
{
	//Validación de parametros de entrada
	if(strcmp(argv[1],"-i")==0 && strcmp(argv[3],"-o")==0 && strcmp(argv[5],"-N")==0 && strcmp(argv[7],"-d")==0)
	{	
		//Comprueba la opción de debug
		if (strcmp(argv[8],"0")==0)
		{
			/* Código sin salida por pantalla */
		}
		else
		{	
			/* Código con salida por pantalla */

		}
	}
	float * repository;
	int num_elements, debug;
	char[] 
	readFile()
	return 0;
}


void readFile(int fileLength, char[] nameOfFile, float *Repository){
	Repository = (float *) malloc(fileLength*sizeof(float));

}