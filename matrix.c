Claude



























Matrix · C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
 
#define MAX 20          // compile-time upper bound for the arrays
 
int N;                  // actual matrix size (from command line)
 
int matA[MAX][MAX];
int matB[MAX][MAX];
 
int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];
 
typedef struct {
    int row;
    int col;
} Cell;
 
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
 
// Sets one cell of matSumResult to matA + matB at that cell.
void* computeSum(void* args) {
    Cell* c = (Cell*)args;
    int r = c->row, col = c->col;
    free(c);
    matSumResult[r][col] = matA[r][col] + matB[r][col];
    return NULL;
}
 
// Sets one cell of matDiffResult to matA - matB at that cell.
void* computeDiff(void* args) {
    Cell* c = (Cell*)args;
    int r = c->row, col = c->col;
    free(c);
    matDiffResult[r][col] = matA[r][col] - matB[r][col];
    return NULL;
}
 
// Sets one cell of matProductResult to (row of A) . (column of B).
void* computeProduct(void* args) {
    Cell* c = (Cell*)args;
    int r = c->row, col = c->col;
    free(c);
    int sum = 0;
    for (int k = 0; k < N; k++) {
        sum += matA[r][k] * matB[k][col];
    }
    matProductResult[r][col] = sum;
    return NULL;
}
 
// Spawns 3 * N * N threads: one per cell for each of the 3 operations.
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
 
    // 3. Create pthread_t objects for our threads.
    pthread_t sumThreads[MAX][MAX];
    pthread_t diffThreads[MAX][MAX];
    pthread_t prodThreads[MAX][MAX];
 
    // 4. Create a thread for each cell of each operation.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Cell* c1 = malloc(sizeof(Cell));
            c1->row = i; c1->col = j;
            pthread_create(&sumThreads[i][j], NULL, computeSum, c1);
 
            Cell* c2 = malloc(sizeof(Cell));
            c2->row = i; c2->col = j;
            pthread_create(&diffThreads[i][j], NULL, computeDiff, c2);
 
            Cell* c3 = malloc(sizeof(Cell));
            c3->row = i; c3->col = j;
            pthread_create(&prodThreads[i][j], NULL, computeProduct, c3);
        }
    }
 
    // 5. Wait for all threads to finish.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            pthread_join(sumThreads[i][j], NULL);
            pthread_join(diffThreads[i][j], NULL);
            pthread_join(prodThreads[i][j], NULL);
        }
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
