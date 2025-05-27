/******************************************************
Integrantes: 
    - Juan Martín Sánchez
    - Juan Sebastián Tellez
    - Nathalia Ramírez
    - Juan Esteban Camargo
    - Santiago Martínez
Materia: Sistemas Operativos
Profesor: John Jairo Corredor
Proyecto: Sistema para el préstamo de libros
Descripción: 
    Archivo de cabecera que declara las funciones públicas del módulo de la base de datos
    (cargar_base_datos, guardar_base_datos, etc.) y define las estructuras Libro y Ejemplar.
    Es utilizado por los demás módulos para interactuar con la base de datos.
******************************************************/

#ifndef SISTEMA_PRESTAMO_H
#define SISTEMA_PRESTAMO_H

typedef struct {
    int numero;
    char estado;
    char fecha[11];
} Ejemplar;

typedef struct {
    char nombre[100];
    int isbn;
    int cantidad;
    Ejemplar ejemplares[10];
} Libro;

int cargar_base_datos(const char *archivo);
int guardar_base_datos(const char *archivo);
int prestar_libro(int isbn);
int devolver_libro(int isbn);
int renovar_libro(int isbn);
Libro* buscar_libro(int isbn);
char* fecha_actual();
char* fecha_mas_7_dias();
extern int total_libros;

#endif