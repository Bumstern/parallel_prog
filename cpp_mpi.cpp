#include "stdio.h"
#include <math.h>
#include <random>
#include "mpi.h"
#include "time.h"


#define PI atan(1)*4


double func(double x) {
    // printf("%f, ", x);
    //return sin(x);
    return 1 / sqrt(2 * PI) * exp(-x * x / 2);
    //return x * x;
}


double integrate(double (*func)(double), double lower, double upper, int iter_num=100) {
    double result = 0;
    for (int i = 0; i < iter_num; ++i) {
        result += func((double)rand()/RAND_MAX * (upper - lower) + lower);
    }
    result *= (upper - lower)/iter_num;
    return result;
}



int main(int argc, char **argv) {
    double answer = 0.997300203936;
    double left = -3, right = 3;
    int precision = 1000;

    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0) {
        printf("Main process starts his work\n");
        int numtasks;
        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
        
        int tries_num = precision;
        for (int i = 1; i < numtasks; ++i) {
            //tries_num *= i;
            MPI_Send(&tries_num, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("Main process send %d to %d process\n", tries_num, i);
        }
        
        double global_result = 0;
        for (int i = 1; i < numtasks; ++i) {
            double result;
            MPI_Recv(&result, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Main process receive %f from %d process\n", result, i);
            global_result += result;
            printf("Global result is %f\n", global_result);
        }
        global_result /= numtasks - 1;

        //MPI_Barrier(MPI_COMM_WORLD);
        printf("\nIntegral of function is: %f\n", global_result);
        printf("Error: %f\n\n", abs(global_result - answer));
        printf("Main process finished his work\n");
    }
    else {
        printf("Process %d starts his work\n", rank);
        int tries_num;
        MPI_Recv(&tries_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d receive %d from main process\n", rank, tries_num);

        srand((unsigned int)(time(NULL) + rank));
        double result = integrate(func, left, right, tries_num);

        MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        printf("Process %d send %f to main process\n", rank, result);
        printf("Process %d finished his work\n", rank);
    }
    
    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
