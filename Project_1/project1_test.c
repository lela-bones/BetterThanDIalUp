#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function Declartions.
void PopulateArray(int *, int, int);
void PrintArray(int *, int);

int main(int argc, char *argv[])
{
    int size, myid, numprocs, i;
    int *ranArry = NULL; //array of random size.
    int range; //Range for the random numbers
    int totalParity, sumParity;
    double PI25DT = 3.14159265358979323846264;
    double mypi, pi, h, sum, x;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    while (1)
    {
        if (myid == 0)
        {
            // If array exists, then free memory for new array
            if (ranArry != NULL)
                free (ranArry);

            // Enter the size of the array and allocs memory.
            printf("Enter the size of the array: (0 quits) ");
            scanf("%d", &size);
            ranArry = (int *)malloc(size * sizeof(int));

            if (size != 0)
            {
                //sets the random range for numbers to be gen.
                srand(time(NULL));
                printf("Enter Range for random numbers: ");
                scanf("%d", &range);

                PopulateArray(ranArry, size, range);
                PrintArray(ranArry, size);
            }
        }
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (size == 0)
        {
            // end program
            break;
        }
        else
        {
            h = 1.0 / (double) size;
            sum = 0.0;
            for (i = myid + 1; i <= size; i += numprocs)
            {
                printf ("Myid: %d, randArray index: %d, vale: %d\n", myid, i-1, ranArry[i-1]);
                x = h * ((double) i - 0.5);
                sum += (4.0 / (1.0 + x * x));
            }
            mypi = h * sum;
            MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            if (myid == 0)
            {
                printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi-PI25DT));

            }

        }
    }
    MPI_Finalize();
    return 0;
}

/**
 * Populate the array with random numbers from the given range.
 */
void PopulateArray(int *arry, int size, int range)
{
    for (int i = 0; i < size; i++)
    {
        arry[i] = rand() % (range + 1);
    }
}

void PrintArray(int *array, int size)
{
    printf("Array Contents: ");
    for (int i = 0; i < size; i++)
    {
        printf("%d, ", array[i]);
    }
    printf("\n");
}
