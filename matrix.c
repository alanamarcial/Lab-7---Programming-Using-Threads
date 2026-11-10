#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20          // compile-time upper bound for the arrays
#define NUM_THREADS 10  // fixed number of worker threads

int N;                  // actual matrix size (from command line)

int matA[MAX][MAX];
int matB[MAX][MAX];

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread id handles rows id, id + NUM_THREADS, id + 2*NUM_THREADS, ...
// Sets each cell in those rows of matSumResult to matA + matB.
void computeSum(int id) {
    for (int i = id; i < N; i += NUM_THREADS) {
        for (int j = 0; j < N; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
}

// Same row partitioning; sets cells of matDiffResult to matA - matB.
void computeDiff(int id) {
    for (int i = id; i < N; i += NUM_THREADS) {
        for (int j = 0; j < N; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
}

// Same row partitioning; sets cells of matProductResult to the
// inner product of a row of matA and a column of matB.
void computeProduct(int id) {
    for (int i = id; i < N; i += NUM_THREADS) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += matA[i][k] * matB[k][j];
            }
            matProductResult[i][j] = sum;
        }
    }
}

// Each of the 10 threads runs this. It receives its thread number
// (malloc'd), frees it, then does its share of all three operations.
void* worker(void* args) {
    int id = *(int*)args;
    free(args);

    computeSum(id);
    computeDiff(id);
    computeProduct(id);
    return NULL;
}

// Spawns exactly NUM_THREADS (10) threads. Each thread computes its own
// set of rows for the sum, difference, and product matrices.
int main(int argc, char* argv[]) {
    srand(time(0));  // Do Not Remove.

    // 0. Get the matrix size from the command line
    if (argc != 2) {
        printf("Usage: %s <matrix size>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    if (N < 1 || N > MAX) {
        printf("Size must be between 1 and %d\n", MAX);
        return 1;
    }

    // 1. Fill the matrices with random values.
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create exactly 10 pthread_t objects.
    pthread_t threads[NUM_THREADS];

    // 4. Create the 10 threads, passing each its thread number.
    for (int t = 0; t < NUM_THREADS; t++) {
        int* id = malloc(sizeof(int));
        *id = t;
        pthread_create(&threads[t], NULL, worker, id);
    }

    // 5. Wait for all threads to finish.
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
}