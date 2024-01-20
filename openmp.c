#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI atan(1)*4


double func(double x) {
    //return sin(x);
    //return x * x;
    return 1 / sqrt(2 * PI) * exp(-x * x / 2);
}


double integrate(double (*func)(double), double lower, double upper, int iter_num) {
    double result = 0;
    for (int i = 0; i < iter_num; ++i) {
        result += (double)func((double)rand()/RAND_MAX * (upper - lower) + lower);
    }
    result *= (double)(upper - lower)/iter_num;
    return result;
}


void main(int argc, char* argv[]) {
    int thread_num;
    if (argc > 1) {
        thread_num = atoi(argv[1]);
        printf("Using thread num: %d\n", thread_num);
    }
    else {
        printf("Using thread num by default: 12\n");
        thread_num = 12;
    }
    
    double lower = -3;
    double upper = 3;
    int precision = 10000;
    double answer = 0.9973;

    double result_sum = 0;

    #pragma omp parallel num_threads(thread_num)
    {
        int rank = omp_get_thread_num();
        double result = integrate(&func, lower, upper, precision);
        printf("Thread %d ended his work. Result: %.5f\n", rank, result);
        result_sum += result;
    }
    double global_result = result_sum / thread_num;

    printf("\nIntegral of function is: %f\n", global_result);
    printf("Error: %f\n\n", global_result - answer);
    exit(0);
}
