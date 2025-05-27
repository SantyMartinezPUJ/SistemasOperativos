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

int cargar_base_datos(const char *archivo);
int guardar_base_datos(const char *archivo);
int prestar_libro(int isbn);
int devolver_libro(int isbn);
int renovar_libro(int isbn);

#endif