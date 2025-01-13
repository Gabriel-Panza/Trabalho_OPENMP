#define TAMANHOLISTA 500000
#define THREADS 2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int* gera_aleatorio(int tamanho_lista) {
    int *lista = (int*)malloc(tamanho_lista * sizeof(int));
    
    for (int i = 0; i < tamanho_lista; i++) {
        lista[i] = rand() % 500000;
    }

    return lista;
}

void printa_lista(int *lista, int tamanho_lista) {

    printf("Os elementos da lista são:\n\n");
    for (int i = 0; i < tamanho_lista; i++) {
        printf("%d ", lista[i]);
    }
    printf("\n");
}

int* bubblesort_paralelo(int *lista, int n) {
    
    int troca;

    int *lista_ordenada = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        lista_ordenada[i] = lista[i];
    }
    

    //enquanto alguma troca ainda for feita ele continua
    //se apenas a 1 interacao ocorrer, entao a lista ja estava ordenada
    //O reduction com OR eh necessario para garantir que se alguma THREAD mudar para 1, o final sera 1
    do{
        troca = 0;

        //o bubble eh dividido em 2 para que nao haja condicao de corrida entre as threads
        //de forma que uma iteracao do for nao interfere na outra

        //percorre os indices pares
        #pragma omp parallel for reduction(|:troca)
        for (int i = 0; i < n - 1; i += 2) {
            if (lista_ordenada[i] > lista_ordenada[i + 1]) {
                int temp = lista_ordenada[i];
                lista_ordenada[i] = lista_ordenada[i + 1];
                lista_ordenada[i + 1] = temp;
                troca = 1;
            }
        }

        //percorre os indices impares
        #pragma omp parallel for reduction(|:troca)
        for (int i = 1; i < n - 1; i += 2) {
            if (lista_ordenada[i] > lista_ordenada[i + 1]) {
                int temp = lista_ordenada[i];
                lista_ordenada[i] = lista_ordenada[i + 1];
                lista_ordenada[i + 1] = temp;
                troca = 1;
            }
        }
    } while (troca);

    return lista_ordenada;
}

void main() {
    double start_time, end_time;

    // garante que uma seed diferente será usada para cada execução para garantir aleatoriedade
    srand((unsigned int)time(NULL));

    int *lista_original = gera_aleatorio(TAMANHOLISTA);
    printa_lista(lista_original,TAMANHOLISTA);

    omp_set_num_threads(THREADS);

    // --------------------------------------------------------------------------
    
    // Inicialização do tempo antes do sort paralelo
    start_time = omp_get_wtime();
    int *lista_ordenada_bubble = bubblesort_paralelo(lista_original, TAMANHOLISTA);
    // Finalização do tempo antes do sort paralelo
    end_time = omp_get_wtime();

    printa_lista(lista_ordenada_bubble,TAMANHOLISTA);

    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    free(lista_original);
    free(lista_ordenada_bubble);
}
