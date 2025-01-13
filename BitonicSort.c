#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <limits.h>

#define tam 500000
#define num_threads 1

#define ASCENDING 1
#define DESCENDING 0

void compare_and_swap(int *arr, int i, int j, int direction) {
    if ((direction == ASCENDING && arr[i] > arr[j]) ||
        (direction == DESCENDING && arr[i] < arr[j])) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void bitonic_sort(int *arr, int n) {
    int i, j, k;
    int power_of_2 = 1;
    while (power_of_2 < n) power_of_2 *= 2;

    int *extended_arr = (int *)malloc(power_of_2 * sizeof(int));
    for (i = 0; i < n; i++) {
        extended_arr[i] = arr[i];
    }
    for (i = n; i < power_of_2; i++) {
        extended_arr[i] = INT_MAX; // Preenche com valores máximos
    }

    for (k = 2; k <= power_of_2; k *= 2) {
        for (j = k / 2; j > 0; j /= 2) {
            #pragma omp parallel for
            for (i = 0; i < power_of_2; i++) {
                int l = i ^ j;
                if (l > i) {
                    int direction = ((i / k) % 2 == 0) ? ASCENDING : DESCENDING;
                    compare_and_swap(extended_arr, i, l, direction);
                }
            }
        }
    }

    for (i = 0; i < n; i++) {
        arr[i] = extended_arr[i];
    }
    free(extended_arr);
}

void generate_random_array(int *arr, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 500000;
    }
}

void printa_lista(int *lista, int tamanho_lista) {

    printf("Os elementos da lista são:\n\n");
    for (int i = 0; i < tamanho_lista; i++) {
        printf("%d ", lista[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    omp_set_num_threads(num_threads);

    int *arr = (int *)malloc(tam * sizeof(int));
    if (arr == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    generate_random_array(arr, tam);

    printa_lista(arr, tam);

    double start_time = omp_get_wtime();
    bitonic_sort(arr, tam);
    double end_time = omp_get_wtime();

    printa_lista(arr, tam);

    printf("Número de threads: %d\n", num_threads);
    printf("Tempo de execução do Bitonic Sort: %.6f segundos\n", end_time - start_time);

    free(arr);
    return 0;
}