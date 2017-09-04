#include <stdio.h>
#include <pmmintrin.h>
#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

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

//Declaración de Funciones
void readFile;
void writeFile;
void sortingNetwork;
void bmnNetwork;
float* mergeSIMD;
void orderFinalMWMS;

/* 
	Bloque de código principal
**Aquí se ejecután las funcionalidades desarrolladas en el orden correspondiente para lograr lo solicitado.
*/
int main(int argc, char **argv)
{
	float *repository, *resultArray;
	int i, num_elements, debug;
	File *inFile, *outFile; 
	/*
		Declaración de registros de tamaño float 
		Cada registro posee 4 numeros float
	*/
	__m128 A, B, C, D; 
	//Validación de parametros de entrada
	if(strcmp(argv[1],"-i")==0 && strcmp(argv[3],"-o")==0 && strcmp(argv[5],"-N")==0 && strcmp(argv[7],"-d")==0)
	{	
		//Set de parámetros de entrada
		inFile       = fopen(argv[2],"r");
		outFile      = fopen(argv[4],"w");
		num_elements = atoi(argv[6]);
		debug        = ato(argv[8]);
		/*
			Reservo memoria para estructura de datos que almacenará el conjunto de secuancias de 16 numeros ordenadas 
			y que será accedida por el Multway Merge Sort para entregar el conjunto completo ordenado.
	 	*/
		resultArray  = (float *) malloc(num_elements*sizeof(float));
		
		//lectura de archivo:
		readFile( num_elements, &inFile, &repository);
		/* Repetir el proceso para cada conjunto de 16 números */
		for (i = 0; i < (int)(num_elements/16); i++)
		{
			//Se leen y asignan desde memoria 4 numeros a cada registro MMX
			A = _mm_load_ps(repository);
			B = _mm_load_ps(repository[4]);
			C = _mm_load_ps(repository[8]);
			D = _mm_load_ps(repository[9]);

			//Se ordenan 4 secuencias de numeros con Sorting Network
			sortingNetwork(A);
			sortingNetwork(B);
			sortingNetwork(C);
			sortingNetwork(D);

			//Se ordenan dos secuencias logicas bitonicas de tamaño 8 con BMN Network
			bmnNetwork(A, B, C, D);

			/*	A esta altura poseo dos cadenas ordenadas de tamaño 8. 
				Se ordena la cadena de 16 numeros a partir de dos cadenas logicas de tamaño 8 usando Merge SIMD 
			*/
			resultArray[i*16] = mergeSIMD(A, B, C, D);
		}

		//Comprueba la opción de debug
		if (debug==0)
		{
			/* Código sin salida por pantalla */
		}
		else
		{	
			/* Código con salida por pantalla */

		}
	}
	else
	{
		fprintf("Error in the execution of the software: Please indicate parameters correctly");
	}
	return 0;
}


void readFile(int fileLength, File *nameOfFile, float *Repository){
	Repository = (float *) malloc(fileLength*sizeof(float));
	read(nameOfFile, Repository, fileLength*sizeof(float));
}

void writeFile(int fileLength, File *nameOfFile, float *Repository){
	write();
}