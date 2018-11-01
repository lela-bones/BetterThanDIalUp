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
    int size, myid, numprocs;
    int *sendcounts, *displs; // Array for how many elements to send and displacement of elements.
    int *ranArry = NULL; //array of random size.
    int *localArray; // used to store random values for each process.
    int rem; // remaining elements after division among processes.
    int sum = 0; // Sum of counts. Used to calculate displacements
    int range; //Range for the random numbers
    int totalParity, sumParity;

    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    while (1)
    {
        if (myid == 0)
        {
            size = 0;
            totalParity = 0;
            // Enter the size of the array and allocs memory.
            printf("Enter the size of the array: (0 quits) ");
            scanf("%d", &size);
        }
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (size == 0)
        {
            // end program
            break;
        }
        else
        {

            localArray = (int *)malloc(((size/numprocs) + 1) * sizeof(int));
            sendcounts = (int *)malloc(numprocs * sizeof(int));
            displs = (int *)malloc(numprocs * sizeof(int));

            if (myid == 0)
            {
                ranArry = (int *)malloc(size * sizeof(int)); // create random array
                srand(time(NULL)); //sets the random range for numbers to be gen.
                printf("Enter Range for random numbers: ");
                scanf("%d", &range);
                
                PopulateArray(ranArry, size, range); // add random numbers to array
                PrintArray(ranArry, size);

                rem = size % numprocs; // remaining slots

                // calculate send counts and displacements
                for (int i = 0; i < numprocs; i++) 
                {
                    sendcounts[i] = size/numprocs;
                    if (rem > 0) 
                    {
                        sendcounts[i]++;
                        rem--;
                    }

                    displs[i] = sum;
                    sum += sendcounts[i];
                }
            }
            MPI_Bcast(sendcounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);

            // send parts of main array to processes.
            MPI_Scatterv(ranArry, sendcounts, displs, MPI_INT, localArray, (size/numprocs) + 1, MPI_INT, 0, MPI_COMM_WORLD);
            
            sumParity = 0; // sum of even numbers for current process
            for (int i = 0; i < sendcounts[myid]; i++)
            {
                //printf ("Myid: %d, randArray index: %d, vale: %d\n", myid, i, localArray[i]);
                if (localArray[i] % 2 == 0)
                    sumParity++;
            }
            MPI_Reduce(&sumParity, &totalParity, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            if (myid == 0)
            {
                printf("***********Number of even numbers: %d\n", totalParity);
            }
            
        }

        if (myid == 0)
        {
            free (ranArry);
        }
        free (localArray);
        free (sendcounts);
        free (displs);
        break;
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

