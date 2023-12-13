#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int MAX;
void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    // 0. Get the matrix size from the command line and assign it to MAX
    printf("Enter the matrix size: ");
    scanf("%d", &MAX);

    int matA[MAX][MAX];
    int matB[MAX][MAX];

    int matSumResult[MAX][MAX];
    int matDiffResult[MAX][MAX];
    int matProductResult[MAX][MAX];

    // struct for args
    struct args_t {
        int (*matA)[MAX];
        int (*matB)[MAX];
        int (*result)[MAX];
        int row;
    };

    // Fetches the appropriate coordinates from the argument, and sets
    // the cell of matSumResult at the coordinates to the sum of the
    // values at the coordinates of matA and matB.
    // pass in the number of the ith thread
    void* computeSum(void* args) {
        struct args_t* arg = (struct args_t*) args;
        int offset = arg->row * MAX;
        for (int i=0; i<MAX; i++) {
            int added = arg->matA[0][i+offset] + arg->matB[0][i+offset];
            arg->result[0][i+offset] = added;
        }
    }

    // Fetches the appropriate coordinates from the argument, and sets
    // the cell of matSumResult at the coordinates to the difference of the
    // values at the coordinates of matA and matB.
    // pass in the number of the ith thread
    void* computeDiff(void* args) {
        struct args_t* arg = (struct args_t*) args;
        int offset = arg->row * MAX;
        for (int i=0; i<MAX; i++) {
            int added = arg->matA[0][i+offset] - arg->matB[0][i+offset];
            arg->result[0][i+offset] = added;
        }
    }

    // Fetches the appropriate coordinates from the argument, and sets
    // the cell of matSumResult at the coordinates to the inner product
    // of matA and matB.
    // pass in the number of the ith thread
    void* computeProduct(void* args) {
        struct args_t* arg = (struct args_t*) args;
        int offset = arg->row * MAX;
        for (int i=0; i<MAX; i++) {
            int added = arg->matA[0][i+offset] * arg->matB[0][i+offset];
            arg->result[0][i+offset] = added;
        }
    }
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create pthread_t objects for our threads.
    pthread_t* addThread = malloc(MAX*MAX*sizeof(pthread_t));
    pthread_t* subtractThread = malloc(MAX*MAX*sizeof(pthread_t));
    pthread_t* multiplyThread = malloc(MAX*MAX*sizeof(pthread_t));
    struct args_t args[MAX];
    // 4. Create a thread for each cell of each matrix operation.
    //
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    //
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.

    //add thread
    for (int i=0; i<MAX; i++) {
        args[i].matA = matA;
        args[i].matB = matB;
        args[i].result = matSumResult;
        args[i].row = i;
        pthread_create(&addThread[i], NULL, &computeSum, (void *) &args[i]);
    }

    //subtract thread
    for (int i=0; i<MAX; i++) {
        args[i].matA = matA;
        args[i].matB = matB;
        args[i].result = matDiffResult;
        args[i].row = i;
        pthread_create(&subtractThread[i], NULL, &computeDiff, (void *) &args[i]);
    }

    //multiply thread
    for (int i=0; i<MAX; i++) {
        args[i].matA = matA;
        args[i].matB = matB;
        args[i].result = matProductResult;
        args[i].row = i;
        pthread_create(&multiplyThread[i], NULL, &computeProduct, (void *) &args[i]);
    }
    // 5. Wait for all threads to finish.
    for (int i=0; i<MAX; i++) {
        pthread_join(addThread[i], NULL);
    }

    for (int i=0; i<MAX; i++) {
        pthread_join(subtractThread[i], NULL);
    }
    for (int i=0; i<MAX*MAX; i++) {
        pthread_join(multiplyThread[i], NULL);
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