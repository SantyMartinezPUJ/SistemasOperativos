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
    Contiene la lógica del proceso receptor (RP), que recibe solicitudes desde los procesos solicitantes. 
    Gestiona directamente los préstamos (P) y delega las renovaciones (R) y devoluciones (D) a un hilo auxiliar mediante un buffer circular sincronizado con semáforos. 
    También posee un hilo de consola que permite cerrar el sistema (s) o generar un reporte (r). Al finalizar, puede guardar el estado final de la base de datos.
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include "SistemaDePrestamoDeLibros.h"

#define MAX_LINE 256
#define N 10

typedef struct {
    char tipo;
    char libro[100];
    int isbn;
} Peticion;

Peticion buffer[N];
int in = 0, out = 0;
sem_t empty, full;
pthread_mutex_t mutex;

int verbose = 0;
int terminar = 0;

void procesar_peticion(Peticion p) {
    if (p.tipo == 'D') {
        printf("[DB] Procesando D - %s (%d)\n", p.libro, p.isbn);
        devolver_libro(p.isbn);
    } else if (p.tipo == 'R') {
        printf("[DB] Procesando R - %s (%d)\n", p.libro, p.isbn);
        renovar_libro(p.isbn);
    }
}

void* hilo_auxiliar(void* arg) {
    while (!terminar || sem_trywait(&full) == 0) {
        if (!terminar) sem_wait(&full);
        pthread_mutex_lock(&mutex);
        Peticion p = buffer[out];
        out = (out + 1) % N;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        procesar_peticion(p);
    }
    return NULL;
}

void* hilo_consola(void* arg) {
    char comando;
    while (!terminar) {
        comando = getchar();
        if (comando == 's') {
            terminar = 1;
        } else if (comando == 'r') {
            printf("[REPORTE] Simulado: mostrar operaciones realizadas\n");
        }
    }
    return NULL;
}

void publicar_en_buffer(Peticion p) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    buffer[in] = p;
    in = (in + 1) % N;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
}

void procesar_linea(char* linea) {
    Peticion p;
    sscanf(linea, " %c,%[^,],%d", &p.tipo, p.libro, &p.isbn);

    if (verbose) printf("[RP] Recibido: %s\n", linea);

    if (p.tipo == 'D' || p.tipo == 'R') {
        publicar_en_buffer(p);
        printf("[RP] Aceptado %c para %s\n", p.tipo, p.libro);
    } else if (p.tipo == 'P') {
        int r = prestar_libro(p.isbn);
        if (r == -1) {
            printf("[RP] Error: El libro con ISBN %d no existe\n", p.isbn);
        } else if (r == 0) {
            printf("[RP] El libro con ISBN %d está agotado\n", p.isbn);
        } else {
            printf("[RP] Procesando préstamo de %s\n", p.libro);
        }
    } else if (p.tipo == 'Q') {
        printf("[RP] PS indica salida\n");
        terminar = 1;
    }
}

int main(int argc, char *argv[]) {
    char *pipe_name = NULL, *archivoBD = NULL, *archivoSalida = NULL;
    int opt;
    char linea[MAX_LINE];

    while ((opt = getopt(argc, argv, "p:f:s:v")) != -1) {
        switch (opt) {
            case 'p': pipe_name = optarg; break;
            case 'f': archivoBD = optarg; break;
            case 's': archivoSalida = optarg; break;
            case 'v': verbose = 1; break;
        }
    }

    if (!pipe_name || !archivoBD) {
        fprintf(stderr, "Uso: %s -p pipe -f archivoBD [-v] [-s archivoSalida]\n", argv[0]);
        exit(1);
    }

    if (cargar_base_datos(archivoBD) == -1) {
        perror("No se pudo cargar la base de datos");
        exit(1);
    }

    int fd = open(pipe_name, O_RDONLY);
    if (fd == -1) {
        perror("Error abriendo pipe");
        exit(1);
    }

    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t aux_thread, consola_thread;
    pthread_create(&aux_thread, NULL, hilo_auxiliar, NULL);
    pthread_create(&consola_thread, NULL, hilo_consola, NULL);

    while (!terminar && read(fd, linea, sizeof(linea)) > 0) {
        char *line = strtok(linea, "\n");
        while (line != NULL) {
            procesar_linea(line);
            line = strtok(NULL, "\n");
        }
        memset(linea, 0, sizeof(linea));
    }

    pthread_join(aux_thread, NULL);
    pthread_join(consola_thread, NULL);

    if (archivoSalida) {
        guardar_base_datos(archivoSalida);
        printf("[RP] Base de datos guardada en %s\n", archivoSalida);
    }

    close(fd);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("[RP] Finalizado.\n");
    return 0;
}
