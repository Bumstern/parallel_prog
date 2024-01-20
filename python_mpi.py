import math
from typing import Callable
from random import random
from random import seed
import time

from mpi4py import MPI


def func(x: float):
    return 1 / math.sqrt(2 * math.pi) * math.exp(-x * x / 2)
    # return math.sin(x)
    # return math.sin(x)/math.exp(math.cos(x))
    # return math.exp(math.sqrt(x))/math.sqrt(x)


def integrate(func: Callable, lower: float, upper: float, iter_num: int=100):
    result = 0
    for i in range(iter_num):
        result += func(random() * (upper - lower) + lower)
    result *= (upper - lower) / iter_num
    return result


def main():
    answer = 0.9973
    left = -3
    right = 3
    precision = 1000

    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()

    if rank == 0:
        print("Main process starts his work")
        numtasks = comm.Get_size()

        tries_num = precision
        for i in range(1, numtasks):
            comm.send(tries_num, dest=i)
            print("Main process send", tries_num, "to", i, "process")

        global_result = 0
        for i in range(1, numtasks):
            result = comm.recv(source=i)
            print("Main process receive", result, "from", i, "process")
            global_result += result
            print("Global result is", global_result)
        global_result /= numtasks - 1

        print("\nIntegral of function is:", global_result)
        print("Error:", abs(global_result - answer), "\n")
        print("Main process finished his work")
    else:
        print("Process", rank, "starts his work")
        tries_num = MPI.COMM_WORLD.recv(source=0)
        print("Process", rank, "receive", tries_num, "from main process")

        seed(time.time() + rank)
        result = integrate(func, left, right, tries_num)

        MPI.COMM_WORLD.send(result, dest=0)
        print("Process", rank, "send", result, "to main process")
        print("Process", rank, "finished his work")
    MPI.Finalize()


if __name__ == '__main__':
    main()
