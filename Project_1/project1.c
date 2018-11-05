/**********************************************************************************************
 * FILENAME project1.c                                      High-Preformance Computing Class.
 *
 * Description:
 *
 *      1: Generates an array of size N of random integers (from 0 to M).
 *          Random integers are inclusive.
 *      2: Counts the number of even parity numbers in the array, and report this number.
 *      3: Counts the distint numbers in the array and report a frequency table.
 *      4: Determines which of the numbers in the array are prime and reports the %
 *          that are prime, as well as a list of those prime numbers.
 *
 * AUTHORS: Matt Matuk, Lela Bones, Preston Mowe... "Spelling
 *
 **********************************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function Declartions.
void PopulateArray(int *, int, int);
void PrintArray(int *, int);
int isPrime(int);

int main(int argc, char *argv[])
{
    int size, myid, numprocs;
    int *sendcounts, *displs, *sendfreqcounts, *freqdispls; // Array for how many elements to send and displacement of elements.
    int *ranArry; //array of random size.
    int *localArray, *localfreqArray, *localPrimeArray; // used to store random values for each process.
    int *localFreqTable; // Store the frequency of distink values.
    int *totalFreqTable, *totalPrimeTable; // store sum of freq
    int not_present; //checks if the number appears in the frequency array
    int rem; // remaining elements after division among processes.
    int sum = 0; // Sum of counts. Used to calculate displacements
    int range; //Range for the random numbers
    int totalParity, sumParity;
    int localfreqZeros, TotalFreqZeros; // stores the local and total amount of zeros in the freq array.
    double startTime = 0.0, endTime;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    while (1)
    {
        if (myid == 0)
        {
            size = 0;
            totalParity = 0;
            TotalFreqZeros = 0;
            // Enter the size of the array and allocs memory.
            printf("Enter the size of the array: (0 quits) ");
            scanf("%d", &size);
        }
        if (myid == 0)
            startTime = MPI_Wtime();
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
            sendfreqcounts = (int *)malloc(numprocs * sizeof(int));
            freqdispls = (int *)malloc(numprocs * sizeof(int));

            // Process 0 will create the random array and determine the size of each of the other
            // processes, include self
            if (myid == 0)
            {
                ranArry = (int *)malloc(size * sizeof(int)); // create random array

                srand(time(NULL)); //sets the random range for numbers to be gen.
                printf("Enter Range for random numbers: ");
                scanf("%d", &range);

                PopulateArray(ranArry, size, range); // add random numbers to array
                //PrintArray(ranArry, size);

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

                sum = 0;
                rem = (range+1) % numprocs; // remaining slots

                // calculate send counts and displacements
                for (int i = 0; i < numprocs; i++)
                {
                    sendfreqcounts[i] = (range+1)/numprocs;
                    if (rem > 0)
                    {
                        sendfreqcounts[i]++;
                        rem--;
                    }

                        freqdispls[i] = sum;

                    sum += sendfreqcounts[i];
                }
                totalPrimeTable = (int *)calloc(range+1, sizeof(int));
                totalFreqTable = (int *)calloc(range+1, sizeof(int));
            }

            MPI_Bcast(sendfreqcounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(freqdispls, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(sendcounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&range, 1, MPI_INT, 0, MPI_COMM_WORLD);

            localFreqTable = (int *)calloc(range + 1, sizeof(int));
            localfreqArray = (int *)malloc(sendfreqcounts[myid] * sizeof(int));
            localPrimeArray = (int *)calloc(range+1, sizeof(int));
            // send parts of main array to processes.
            MPI_Scatterv(ranArry, sendcounts, displs, MPI_INT, localArray, (size/numprocs) + 1, MPI_INT, 0, MPI_COMM_WORLD);

            sumParity = 0; // sum of even numbers for current process
            for (int i = 0; i < sendcounts[myid]; i++)
            {
                //printf ("Myid: %d, randArray index: %d, vale: %d\n", myid, i, localArray[i]);
                if (localArray[i] % 2 == 0)
                    sumParity++;

                // Increase freq.
                localFreqTable[localArray[i]]++;
            }


            MPI_Reduce(&sumParity, &totalParity, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // give process 0 sum of parity
            MPI_Reduce(localFreqTable, totalFreqTable, range +1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // gives process 0 total freq table

            MPI_Scatterv(totalFreqTable, sendfreqcounts, freqdispls, MPI_INT, localfreqArray, sendfreqcounts[myid], MPI_INT, 0, MPI_COMM_WORLD);

            //***************************************
            // Find the amount of zeros in feq array and determines what numbers are prime.
            //***************************************
            localfreqZeros = 0;
            int totalPrime = 0;
            int prime = 0;
            for (int i = 0; i < sendfreqcounts[myid]; i++)
            {
                //printf ("Myid: %d, randArray index: %d, vale: %d\n", myid, i, localArray[i]);
                if (localfreqArray[i] == 0)
                    localfreqZeros++;
                else{
                    if(isPrime(freqdispls[myid]+i))
                    {
                        //printf("Prime found: myid: %d, Number: %d,freq: %d\n", myid, freqdispls[myid]+i, localfreqArray[i]);
                        prime += localfreqArray[i];
                        localPrimeArray[i+freqdispls[myid]] = 1;
                    }
                }

                //TODO: determine what number are prime. Remember that the localfreqarray index locations will
                // be from 0-n for each process. Need to account for that what picking the number to determine
                // if it is prime. Could Bcast eh freqdispls array and add the offsets to the current localfreqarray index.
            }

            MPI_Reduce(localPrimeArray, totalPrimeTable, range+1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // gives process 0 total freq table
            MPI_Reduce(&localfreqZeros, &TotalFreqZeros, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // gives process 0 total freq table
            MPI_Reduce(&prime, &totalPrime, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // give process 0 sum of primes

            //MPI_Barrier(MPI_COMM_WORLD); // Might not be needed, but just wanted to make sure all process got here before printing results
            //Display results
            double precentPrime = 0.0;
            if (myid == 0)
            {
                precentPrime = ((float)totalPrime/(float)size) * 100;
                endTime = MPI_Wtime();
                //printf("size: %d\n", size);
                printf("\n\n");
                printf("//*************************************************************\\\\\n");
                printf("||                       results                               ||\n");
                printf("\\\\*************************************************************//\n\n");

                printf("Total Data: %d\n", size);
                printf("Number of even numbers: %d\n", totalParity);
                printf("Freq Table: ");
                PrintArray(totalFreqTable, range + 1);
                printf("Total Distint Numbers: %d\n", range+1 - TotalFreqZeros);
                printf("Total Prime Numbers: %d\n", totalPrime);
                printf("List of Primes: ");
                for (int i = 0; i < range+1; i ++)
                {
                    if(totalPrimeTable[i] == 1)
                    {
                        printf("%d, ", i);
                    }
                }
                printf("\n");
                printf("Total Percent of Primes: %.2f%%\n", precentPrime);
                printf("wall clock time = %f seconds\n", endTime - startTime);
            }

        }
        break;
    }

    //Free up used memory
    if (myid == 0)
    {
        free (ranArry);
        free (totalFreqTable);
    }
    free (localArray);
    free (sendcounts);
    free (displs);
    free (localFreqTable);

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
    for (int i = 0; i < size-1; i++)
    {
        printf("%d, ", array[i]);
    }
    printf("%d", array[size-1]);
    printf("\n");
}

/**
 * Determines if the passed int is prime.
 *
 * Retuns 1 if prime, else 0.
 */
int isPrime(int num)
{
    if (num == 1 || num == 2)
        return 1;
    if (num == 0)
        return 0;

    int i = 2;
    int x = num; // num divided by i
    int rem = 0;

    while (i <= x)
    {
        rem = num % i;

        if (rem == 0)
            return 0;
        i++;
    }
    return 1;
}
