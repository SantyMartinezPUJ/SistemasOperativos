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
    Módulo encargado de la gestión de la base de datos de libros. Implementa funciones para cargar los libros desde un archivo,
    procesar préstamos (prestar_libro), devoluciones (devolver_libro) y renovaciones (renovar_libro), así como guardar el estado actualizado en un archivo de salida.
    Internamente maneja estructuras para representar libros y ejemplares.
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SistemaDePrestamoDeLibros.h"

#define MAX_LIBROS 100
#define MAX_EJEMPLARES 10

Libro biblioteca[MAX_LIBROS];
int total_libros = 0;

// Devuelve la fecha actual formateada como dd-mm-aaaa
char* fecha_actual() {
    static char fecha[11];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(fecha, sizeof(fecha), "%02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    return fecha;
}

// Devuelve la fecha actual + 7 días, usada para renovaciones
char* fecha_mas_7_dias() {
    static char fecha[11];
    time_t t = time(NULL);
    t += 7 * 24 * 60 * 60;  // sumar 7 días en segundos
    struct tm tm = *localtime(&t);
    snprintf(fecha, sizeof(fecha), "%02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    return fecha;
}

// Carga los libros y ejemplares desde el archivo de texto
int cargar_base_datos(const char *archivo) {
    FILE *f = fopen(archivo, "r");
    if (!f) {
        perror("Error abriendo archivo de base de datos");
        return -1;
    }

    total_libros = 0;
    char linea[256];

    while (fgets(linea, sizeof(linea), f)) {
        Libro *libro = &biblioteca[total_libros];

        // Leer encabezado del libro: nombre, ISBN, cantidad
        if (sscanf(linea, " %[^,],%d,%d", libro->nombre, &libro->isbn, &libro->cantidad) != 3) {
            continue;
        }

        printf("[DEBUG] Libro cargado: %s, ISBN=%d, ejemplares=%d\n", libro->nombre, libro->isbn, libro->cantidad);

        // Leer ejemplares del libro
        for (int i = 0; i < libro->cantidad; i++) {
            if (fgets(linea, sizeof(linea), f)) {
                sscanf(linea, "%d,%c,%s",
                       &libro->ejemplares[i].numero,
                       &libro->ejemplares[i].estado,
                       libro->ejemplares[i].fecha);
            }
        }

        total_libros++;
    }

    fclose(f);
    return 0;
}

// Guarda el estado actual de la base de datos en un archivo
int guardar_base_datos(const char *archivo) {
    FILE *f = fopen(archivo, "w");
    if (!f) {
        perror("No se pudo abrir el archivo de salida");
        return -1;
    }

    if (total_libros == 0) {
        printf("[DEBUG] Advertencia: No hay libros cargados para guardar.\n");
    } else {
        printf("[DEBUG] Guardando %d libros en %s\n", total_libros, archivo);
    }

    // Escribir encabezados y ejemplares por libro
    for (int i = 0; i < total_libros; i++) {
        Libro *libro = &biblioteca[i];
        fprintf(f, "%s,%d,%d\n", libro->nombre, libro->isbn, libro->cantidad);

        for (int j = 0; j < libro->cantidad; j++) {
            fprintf(f, "%d,%c,%s\n",
                libro->ejemplares[j].numero,
                libro->ejemplares[j].estado,
                libro->ejemplares[j].fecha);
        }
    }

    fclose(f);
    return 0;
}

// Busca un libro en la base por su ISBN
Libro* buscar_libro(int isbn) {
    for (int i = 0; i < total_libros; i++) {
        printf("[DEBUG] Revisando libro: %s, ISBN=%d\n", biblioteca[i].nombre, biblioteca[i].isbn);
        if (biblioteca[i].isbn == isbn) return &biblioteca[i];
    }
    return NULL;
}

// Registra el préstamo de un ejemplar disponible
int prestar_libro(int isbn) {
     printf("[DEBUG] Buscando ISBN %d en biblioteca...\n", isbn);
    Libro *libro = buscar_libro(isbn);
    if (!libro) {
        printf("[DB] Error: No se encontró el libro con ISBN %d\n", isbn);
        return -1;
    }
    for (int i = 0; i < libro->cantidad; i++) {
        if (libro->ejemplares[i].estado == 'D') {
            libro->ejemplares[i].estado = 'P';
            strcpy(libro->ejemplares[i].fecha, fecha_actual());
            return 1;
        }
    }

    printf("[DB] No hay ejemplares disponibles para %s\n", libro->nombre);
    return 0;
}

// Registra la devolución de un ejemplar prestado
int devolver_libro(int isbn) {
    Libro *libro = buscar_libro(isbn);
    if (!libro) return -1;
    for (int i = 0; i < libro->cantidad; i++) {
        if (libro->ejemplares[i].estado == 'P') {
            libro->ejemplares[i].estado = 'D';
            strcpy(libro->ejemplares[i].fecha, fecha_actual());
            return 1;
        }
    }
    return 0;
}

// Renueva un ejemplar prestado (extiende la fecha 7 días)
int renovar_libro(int isbn) {
    Libro *libro = buscar_libro(isbn);
    if (!libro) return -1;
    for (int i = 0; i < libro->cantidad; i++) {
        if (libro->ejemplares[i].estado == 'P') {
            strcpy(libro->ejemplares[i].fecha, fecha_mas_7_dias());
            return i;
        }
    }
    return -1;
}