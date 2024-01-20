#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define PI atan(1)*4


double func(double x) {
    //return sin(x);
    //return x * x;
    return 1 / sqrt(2 * PI) * exp(-x * x / 2);
}


typedef struct {
    double (*func)(double);
    double upper;
    double lower;
    int iter_num;
    double result;
} intregral_struct;


void* integrate(void *ptr) {
    //pthread_mutex_lock(&mutex);
    intregral_struct* th_data = (intregral_struct*)ptr;

    double result = 0;
    for (int i = 0; i < th_data->iter_num; ++i) {
        result += (double)th_data->func((double)rand()/RAND_MAX * (th_data->upper - th_data->lower) + th_data->lower);
    }
    result *= (double)(th_data->upper - th_data->lower)/th_data->iter_num;

    printf("%f\n", result);
    th_data->result = result;
    //pthread_mutex_unlock(&mutex);
}


int main(int argc, char* argv[]) {
    int thread_num;
    if (argc > 1) {
        thread_num = atoi(argv[1]);
        printf("Using thread num: %d\n", thread_num);
    }
    else {
        printf("Using thread num by default: 12\n");
        thread_num = 12;
    }

    double upper = 3;
    double lower = -3;
    int precision = 10000;
    double answer = (double)0.9973;

    pthread_t thread_list[thread_num];
    intregral_struct* thread_data_list = (intregral_struct*) malloc((size_t)thread_num * sizeof(intregral_struct));

    for (int th_i = 0; th_i < thread_num; ++th_i) {
        srand(time(NULL) + th_i);
        thread_data_list[th_i] = (intregral_struct){&func, upper, lower, precision, 0.};

        pthread_create(&thread_list[th_i], NULL, &integrate, &thread_data_list[th_i]);
        printf("Thread %d created\n", th_i);
    }

    double result_sum = 0;
    for (int th_i = 0; th_i < thread_num; ++th_i) {
        pthread_join(thread_list[th_i], NULL);
        printf("Thread %d end his work. Result: %.8f\n", th_i, thread_data_list[th_i].result);
        result_sum += thread_data_list[th_i].result;
    }
    double global_result = result_sum / thread_num;

    printf("\nIntegral of function is: %f\n", global_result);
    printf("Error: %f\n\n", global_result - answer);
    free(thread_data_list);
    exit(0);
}
