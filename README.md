# Laboratorio 1 HPC

- Alumno: Joaquín Ignacio Villagra Pacheco
- Profesor: Fernando Rannou Fuentes 
- Ayudante: Miguel Cárcamo

## Obejtivos
- Conocer, usar y apreciar las características de paralelismo a nivel de instrucción de un procesador moderno. 
- Implementación de un sistema Mono-Hebreado para ordenamiento de números.

## Técnicas
- Merge Bitónico a nivel SIMD para ordenar subsecuencias y luego Merge a nivel de proceso para mezclar subsecuencias y obtener la secuencia ordenada. 

## Compilación
- Mediante MakeFile

- $ gcc -msse3 -o simdsort simdsort.c $