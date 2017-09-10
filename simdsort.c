#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pmmintrin.h>

void red_ordenamiento(float * listaNumeros, int creciente);
void red_ordenamiento_bitonica(float* a, float* b);
void fusion_SIMD_secuencias_numericas(float* lista_numerica);
float* ordenamiento_secuencias_tamano_16(float* lista_numerica, int tamano);
float* readNumFile(FILE* archivo,int tamano);

typedef struct lista
{
	float * array_numeros;
	int indice;
} listado;


/* 
	* Laboratorio 1 HPC
	* Joaquín Ignacio Villagra Pacheco
	* Obejtivo: Conocer, usar y apreciar las características de paralelismo a nivel de instrucción 
		de un procesador moderno. 
		Implementación de un sistema Mono-Hebreado para ordenamiento de números.
		Merge Bitónico a nivel SIMD para ordenar subsecuencias y luego Merge a nivel de proceso para mezclar
		subsecuencias y obtener la secuencia ordenada. 
	* Profesor Fernando Rannou Fuentes
	* Ayudante Miguel Cárcamo 
*/

int main(int argc, char * const argv[])
{
	int isdebug  = 0;
	int cantidadElementos = 0;
	char *archivo_entrada = NULL;
	char *archivo_salida = NULL;
	float* lista;
	float* listaOrdenada;
	int i, indice, c;
	opterr = 0;
	while ((c = getopt (argc, argv, "i:o:N:d:")) != -1)
		switch (c)
		{
			case 'i':
				archivo_entrada = optarg;
				break;
			case 'o':
				archivo_salida = optarg;
				break;
			case 'N':
				cantidadElementos = atoi(optarg);
				break;
			case 'd':
				isdebug = atoi(optarg);
				break;
			case '?':
				if (optopt == 'i' ||optopt == 'o' ||optopt == 'N' ||optopt == 'd' )
					fprintf (stderr, "La opcion -%c  debe ir acompañada de un argumento.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "No se reconoce opcion `-%c'.\n", optopt);
				else
					fprintf (stderr,
									"No se reconoce opcion `\\x%x'.\n",
									optopt);
				return 1;
			default:
				abort ();
		}

	FILE *entrada = fopen(archivo_entrada,"r");
	lista = readNumFile(f,cantidadElementos);
	fclose(entrada);
	listaOrdenada = ordenamiento_secuencias_tamano_16(lista,cantidadElementos);
	if(isdebug){
		for (i = 0; i < cantidadElementos ; i++) { printf("| %.2f\t| %.2f\t|\n",lista[i],listaOrdenada[i]); }
	}
	FILE *salida = fopen(archivo_salida,"wb");
	fwrite(listaOrdenada,cantidadElementos, sizeof(float),salida);
	fclose(salida);
	return 0;
}

/**
	@function: red_ordenamiento
	@inputs:
		@listaNumeros
		@up: Flag a utilizar pata determinar de que forma dejar la salida de red_ordenamiento,
		ya sea ascendente o descendente. 
**/
void red_ordenamiento(float * listaNumeros, int up){
	__m128d registerA, registerB, registerC, registerD, registerE;
	//Carga en registro listado de numeros a ordenar en red_ordenamiento
	registerA = _mm_load_ps(listaNumeros);

	//Shufle y posterior ejecución de primer min - max
	registerB = _mm_shuffle_ps(registerA, registerA, _MM_SHUFFLE(1,0,3,2));
	registerD = _mm_min_ps(registerB,registerA);
	registerE = _mm_max_ps(registerB,registerA);
	if(!up)//Se comprueba flag para determinar orden a entregar 
		registerA = _mm_shuffle_ps(registerE, registerD, _MM_SHUFFLE(3,2,1,0));
	else
		registerA = _mm_shuffle_ps(registerD, registerE, _MM_SHUFFLE(3,2,1,0));

	//Shufle y posterior ejecución de segunda instancia de Min-Max
	registerB = _mm_shuffle_ps(registerA, registerA, _MM_SHUFFLE(3,1,2,0));
	registerC = _mm_shuffle_ps(registerA, registerA, _MM_SHUFFLE(2,0,3,1));
	registerD = _mm_min_ps(registerB,registerC);
	registerE = _mm_max_ps(registerB,registerC);	
	if(!up)//Se comprueba flag para determinar orden a entregar. 
		registerA = _mm_shuffle_ps(registerE, registerD, _MM_SHUFFLE(3,2,1,0));
	else
		registerA = _mm_shuffle_ps(registerD, registerE, _MM_SHUFFLE(3,2,1,0));
	registerA = _mm_shuffle_ps(registerA, registerA, _MM_SHUFFLE(3,1,2,0));

	/**
		Ultima cadena de Shuffle más min-max
	**/
	registerB = _mm_shuffle_ps(registerA, registerA, _MM_SHUFFLE(3,1,2,0));
	registerD = _mm_min_ps(registerB,registerA);
	registerE = _mm_max_ps(registerB,registerA);
	if(!up)//Se comprueba flag para determinar orden a entregar. 
		registerA = _mm_shuffle_ps(registerE, registerD, _MM_SHUFFLE(3,2,1,0));
	else
		registerA = _mm_shuffle_ps(registerD, registerE, _MM_SHUFFLE(3,2,1,0));

	//Salida de operaciones almacenada
	_mm_store_ps(listaNumeros,registerA);
}

void red_ordenamiento_bitonica(float* listA, float* listB){

	__m128d registerB, registerC, registerD, registerE, registerF, registerG, registerH, registerI, registerJ, registerJ;
	/** 
	**** Ordenamiento necesario para poder operar correctamente la merge network bitonica
	**** Se genera las secuencias bitonicas
	**/
	red_ordenamiento(listA,1);
	red_ordenamiento(listB,0);
	registerB = _mm_load_ps(listA);
	registerC = _mm_load_ps(listB);

	/** 
	**** Primer bloque de combinatoria min-max-shuffle
	**/
	registerD = _mm_shuffle_ps(registerB,registerC,_MM_SHUFFLE(2,0,2,0)); 
	registerE = _mm_shuffle_ps(registerB,registerC,_MM_SHUFFLE(3,1,3,1));
	registerF = _mm_shuffle_ps(registerD, registerD, _MM_SHUFFLE(1,0,3,2));
	registerI = _mm_shuffle_ps(registerE, registerE, _MM_SHUFFLE(1,0,3,2));
	registerG = _mm_min_ps(registerF,registerD);
	registerJ = _mm_min_ps(registerI,registerE);
	registerH = _mm_max_ps(registerF,registerD);
	registerJ = _mm_max_ps(registerI,registerE);
	registerB = _mm_shuffle_ps(registerG, registerH, _MM_SHUFFLE(3,2,1,0));
	registerC = _mm_shuffle_ps(registerJ, registerJ, _MM_SHUFFLE(3,2,1,0));
	registerB = _mm_shuffle_ps(registerB,registerB,_MM_SHUFFLE(3,1,2,0));
	registerC = _mm_shuffle_ps(registerC,registerC,_MM_SHUFFLE(3,1,2,0));

	/** 
	**** Segundo bloque de combinatoria min-max-shuffle
	**/
	registerF = _mm_shuffle_ps(registerB, registerB, _MM_SHUFFLE(1,0,3,2));
	registerI = _mm_shuffle_ps(registerC, registerC, _MM_SHUFFLE(1,0,3,2));
	registerG = _mm_min_ps(registerF,registerB);
	registerJ = _mm_min_ps(registerI,registerC);
	registerH = _mm_max_ps(registerF,registerB);
	registerJ = _mm_max_ps(registerI,registerC);
	registerB = _mm_shuffle_ps(registerG, registerH, _MM_SHUFFLE(3,2,1,0));
	registerC = _mm_shuffle_ps(registerJ, registerJ, _MM_SHUFFLE(3,2,1,0));

	/** 
	**** Ultimo bloque de combinatoria min-max-shuffle
	**/
	registerD = _mm_shuffle_ps(registerB,registerC,_MM_SHUFFLE(2,0,2,0)); 
	registerE = _mm_shuffle_ps(registerB,registerC,_MM_SHUFFLE(3,1,3,1));
	registerF = _mm_shuffle_ps(registerD, registerD, _MM_SHUFFLE(1,0,3,2));
	registerI = _mm_shuffle_ps(registerE, registerE, _MM_SHUFFLE(1,0,3,2));
	registerG = _mm_min_ps(registerF,registerD);
	registerJ = _mm_min_ps(registerI,registerE);
	registerH = _mm_max_ps(registerF,registerD);
	registerJ = _mm_max_ps(registerI,registerE);
	registerB = _mm_shuffle_ps(registerG, registerH, _MM_SHUFFLE(3,2,1,0));
	registerC = _mm_shuffle_ps(registerJ, registerJ, _MM_SHUFFLE(3,2,1,0));
	registerB = _mm_shuffle_ps(registerB,registerB,_MM_SHUFFLE(3,1,2,0));
	registerC = _mm_shuffle_ps(registerC,registerC,_MM_SHUFFLE(3,1,2,0));

	/** 
	**** Almacenaje de resultado
	**/
	_mm_store_ps(a,registerB);
	_mm_store_ps(b,registerC);
}

/**
	@function: 
	@input
		@lista_numerica: Arreglo de tamaño 16 alineado
**/
void fusion_SIMD_secuencias_numericas(float* lista_numerica){
	red_ordenamiento_bitonica(&lista_numerica[0],&lista_numerica[8]);
	red_ordenamiento_bitonica(&lista_numerica[4],&lista_numerica[12]);
	red_ordenamiento_bitonica(&lista_numerica[0],&lista_numerica[4]);
	red_ordenamiento_bitonica(&lista_numerica[8],&lista_numerica[12]);
	red_ordenamiento_bitonica(&lista_numerica[4],&lista_numerica[8]);
}

float* ordenamiento_secuencias_tamano_16(float* lista_numerica, int tamano){
	/**
		Se comprueba si el tamaño del archivo es consistente
		Debe ser multiplo de 16 en este caso.
	**/
	if(tamano%16!=0){
		printf("Para efectos de procesamiento se requiere una lista de numeros con cantidad de elementos multiplo de 16.\n");
		return NULL;
	}

	listado* listasProcesadas = (listado*) malloc(sizeof(listado)*tamano/16);
	float* listado = (float*) malloc(sizeof(float)*tamano);
	int i,j,checklist;
	listado* punteroLista;

	for(i=0;i<tamano/16;i++) {
		fusion_SIMD_secuencias_numericas(&lista_numerica[16*i]);
		listasProcesadas[i].array_numeros=&lista_numerica[16*i];
		listasProcesadas[i].indice=0;
	}
	//Se realiza Merge necesario
	j=0;
	for(checklist=0;checklist<tamano/16;)
	{
		punteroLista=NULL;
		for(i=0;i<tamano/16;i++){
			if(listasProcesadas[i].indice==16) continue;
			if(punteroLista==NULL)
				punteroLista = &listasProcesadas[i];
			if(punteroLista->array_numeros[punteroLista->indice]>listasProcesadas[i].array_numeros[listasProcesadas[i].indice])
				punteroLista = &listasProcesadas[i];
		}
		listado[j]=punteroLista->array_numeros[punteroLista->indice];
		punteroLista->indice++;
		if(punteroLista->indice==16)
			checklist++;
		j++;
	}
	return listado;
}

float* readNumFile(FILE* archivo, int tamano){
	float* elementos;
	posix_memalign((void **) &elementos, 16, tamanoof(float)*tamano);
	fread(elementos, tamano, sizeof(float), archivo);
	return elementos;
}