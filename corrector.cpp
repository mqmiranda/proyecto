#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "corrector.h"

#define ALFABETO "abcdefghijklmnopqrstuvwxyzÃ±Ã‘'&$#!%<[]>â€--+/\"_:?Â¿Â¡@="
#define MAX_PALABRAS 50000

// Crear diccionario
void GenerarDiccionario(char* nombreArchivo, char palabrasDiccionario[][TAMTOKEN], int estadisticas[], int& totalElementos) {
    FILE* archivo;
    if (fopen_s(&archivo, nombreArchivo, "r") != 0) {
        printf("No se puede abrir el archivo: %s.\n", nombreArchivo);
        return;
    }

    char bufferPalabra[TAMTOKEN];
    totalElementos = 0;

    while (fscanf_s(archivo, "%49s", bufferPalabra, (unsigned)_countof(bufferPalabra)) != EOF) {
        int inicio = 0;
        for (int i = 0; bufferPalabra[i]; i++) {
            if (isspace((unsigned char)bufferPalabra[i]) || strchr(",.;()[]", bufferPalabra[i])) {
                if (i > inicio) {
                    char subPalabra[TAMTOKEN] = { 0 };
                    int indexSubPalabra = 0;
                    for (int k = inicio; k < i && indexSubPalabra < TAMTOKEN - 1; k++) {
                        subPalabra[indexSubPalabra++] = tolower((unsigned char)bufferPalabra[k]);
                    }
                    subPalabra[indexSubPalabra] = '\0';

                    if (strlen(subPalabra) > 0) {
                        int existe = -1;
                        for (int j = 0; j < totalElementos; j++) {
                            if (strcmp(palabrasDiccionario[j], subPalabra) == 0) {
                                existe = j;
                                break;
                            }
                        }

                        if (existe == -1 && totalElementos < NUMPALABRAS) {
                            strcpy_s(palabrasDiccionario[totalElementos], TAMTOKEN, subPalabra);
                            estadisticas[totalElementos] = 1;
                            totalElementos++;
                        }
                        else if (existe != -1) {
                            estadisticas[existe]++;
                        }
                    }
                }
                inicio = i + 1;
            }
        }

        if (bufferPalabra[inicio]) {
            char subPalabra[TAMTOKEN] = { 0 };
            int indexSubPalabra = 0;
            for (int k = inicio; bufferPalabra[k] && indexSubPalabra < TAMTOKEN - 1; k++) {
                subPalabra[indexSubPalabra++] = tolower((unsigned char)bufferPalabra[k]);
            }
            subPalabra[indexSubPalabra] = '\0';

            if (strlen(subPalabra) > 0) {
                int existe = -1;
                for (int j = 0; j < totalElementos; j++) {
                    if (strcmp(palabrasDiccionario[j], subPalabra) == 0) {
                        existe = j;
                        break;
                    }
                }

                if (existe == -1 && totalElementos < NUMPALABRAS) {
                    strcpy_s(palabrasDiccionario[totalElementos], TAMTOKEN, subPalabra);
                    estadisticas[totalElementos] = 1;
                    totalElementos++;
                }
                else if (existe != -1) {
                    estadisticas[existe]++;
                }
            }
        }
    }
    fclose(archivo);

    // Ordenar  alfabÃ©ticamente
    for (int i = 0; i < totalElementos - 1; i++) {
        for (int j = i + 1; j < totalElementos; j++) {
            if (strcmp(palabrasDiccionario[i], palabrasDiccionario[j]) > 0) {
                char temporal[TAMTOKEN];
                strcpy_s(temporal, TAMTOKEN, palabrasDiccionario[i]);
                strcpy_s(palabrasDiccionario[i], TAMTOKEN, palabrasDiccionario[j]);
                strcpy_s(palabrasDiccionario[j], TAMTOKEN, temporal);

                int temporalEstadistica = estadisticas[i];
                estadisticas[i] = estadisticas[j];
                estadisticas[j] = temporalEstadistica;
            }
        }
    }
}

// clonacion

void GenerarPalabrasSimilares(char* palabraOriginal, char palabrasSugeridas[][TAMTOKEN], int& totalSugeridas) {
    totalSugeridas = 0;
    int longitud = strlen(palabraOriginal);

    // EliminaciÃ³n de caracteres
    for (int i = 0; i < longitud; i++) {
        char copia[TAMTOKEN];
        strcpy_s(copia, TAMTOKEN, palabraOriginal);
        memmove(&copia[i], &copia[i + 1], longitud - i);
        strcpy_s(palabrasSugeridas[totalSugeridas++], TAMTOKEN, copia);
    }

    // TransposiciÃ³n de los caracteres
    for (int i = 0; i < longitud - 1; i++) {
        char copia[TAMTOKEN];
        strcpy_s(copia, TAMTOKEN, palabraOriginal);
        char temp = copia[i];
        copia[i] = copia[i + 1];
        copia[i + 1] = temp;
        strcpy_s(palabrasSugeridas[totalSugeridas++], TAMTOKEN, copia);
    }

    // SustituciÃ³n de los caracteres
    for (int i = 0; i < longitud; i++) {
        for (int j = 0; j < strlen(ALFABETO); j++) {
            char copia[TAMTOKEN];
            strcpy_s(copia, TAMTOKEN, palabraOriginal);
            copia[i] = ALFABETO[j];
            strcpy_s(palabrasSugeridas[totalSugeridas++], TAMTOKEN, copia);
        }
    }

    // InserciÃ³n de los caracteres
    for (int i = 0; i <= longitud; i++) {
        for (int j = 0; j < strlen(ALFABETO); j++) {
            char copia[TAMTOKEN];
            strncpy_s(copia, TAMTOKEN, palabraOriginal, i);
            copia[i] = ALFABETO[j];
            strcpy_s(&copia[i + 1], TAMTOKEN - i - 1, &palabraOriginal[i]);
            strcpy_s(palabrasSugeridas[totalSugeridas++], TAMTOKEN, copia);
        }
    }
}


//  lista de palabras candidatas
void GenerarListaCandidatas(char palabrasSugeridas[][TAMTOKEN], int totalSugeridas, char palabrasDiccionario[][TAMTOKEN],
    int estadisticas[], int totalElementos, char listaFinal[][TAMTOKEN],
    int pesos[], int& totalFinal) {
    totalFinal = 0;
    for (int i = 0; i < totalSugeridas; i++) {
        for (int j = 0; j < totalElementos; j++) {
            if (strcmp(palabrasSugeridas[i], palabrasDiccionario[j]) == 0) {
                bool duplicado = false;
                for (int k = 0; k < totalFinal; k++) {
                    if (strcmp(listaFinal[k], palabrasDiccionario[j]) == 0) {
                        duplicado = true;
                        break;
                    }
                }
                if (!duplicado) {
                    strcpy_s(listaFinal[totalFinal], TAMTOKEN, palabrasDiccionario[j]);
                    pesos[totalFinal] = estadisticas[j];
                    totalFinal++;
                }
            }
        }
    }

    // Ordenar por peso descendente
    for (int i = 0; i < totalFinal - 1; i++) {
        for (int j = i + 1; j < totalFinal; j++) {
            if (pesos[i] < pesos[j]) {
                int pesoTemp = pesos[i];
                pesos[i] = pesos[j];
                pesos[j] = pesoTemp;

                char palabraTemp[TAMTOKEN];
                strcpy_s(palabraTemp, TAMTOKEN, listaFinal[i]);
                strcpy_s(listaFinal[i], TAMTOKEN, listaFinal[j]);
                strcpy_s(listaFinal[j], TAMTOKEN, palabraTemp);
            }
        }
    }
}
