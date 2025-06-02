#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define THRESHOLD 4096

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void bitonic_compare(int *arr, int low, int count, int dir) {
    int k = count / 2;
    for (int i = low; i < low + k; i++) {
        if ((dir == 1 && arr[i] > arr[i + k]) || (dir == 0 && arr[i] < arr[i + k])) {
            swap(&arr[i], &arr[i + k]);
        }
    }
}

void bitonic_merge(int *arr, int low, int count, int dir) {
    if (count > 1) {
        int k = count / 2;

        bitonic_compare(arr, low, count, dir);

        if (count > THRESHOLD) {
            #pragma omp task
            bitonic_merge(arr, low, k, dir);

            #pragma omp task
            bitonic_merge(arr, low + k, k, dir);

            #pragma omp taskwait
        } else {
            bitonic_merge(arr, low, k, dir);
            bitonic_merge(arr, low + k, k, dir);
        }
    }
}


void bitonic_sort_rec(int *arr, int low, int count, int dir) {
    if (count > 1) {
        int k = count / 2;

        if (count > THRESHOLD) {
            #pragma omp task shared(arr)
            bitonic_sort_rec(arr, low, k, 1);

            #pragma omp task shared(arr)
            bitonic_sort_rec(arr, low + k, k, 0);

            #pragma omp taskwait
        } else {
            bitonic_sort_rec(arr, low, k, 1);
            bitonic_sort_rec(arr, low + k, k, 0);
        }

        bitonic_merge(arr, low, count, dir);
    }
}

void bitonic_sort(int *arr, int n, int num_threads) {
    omp_set_num_threads(num_threads);

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        bitonic_sort_rec(arr, 0, n, 1);
    }

    double end_time = omp_get_wtime();

    printf("Tempo gasto na função bitonic_sort: %.6f segundos\n", end_time - start_time);
}



void print_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void generate_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000000;
    }
}

int next_power_of_2(int n) {
    int power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <número de threads> <tamanho do array>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int n = atoi(argv[2]);

    int next_power = next_power_of_2(n);
    int *arr = malloc(next_power * sizeof(int));
    if (arr == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    srand(time(NULL));
    generate_array(arr, n);

    for (int i = n; i < next_power; i++) {
        arr[i] = INT_MAX;
    }

    //printf("Array original:\n");
    //print_array(arr, n);

    // Ordena o array usando Bitonic Sort
    bitonic_sort(arr, next_power, num_threads);

    //printf("Array ordenado:\n");
    //print_array(arr, n); // Imprime apenas os valores originais

    free(arr);
    return 0;
}