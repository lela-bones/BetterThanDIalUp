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
    int *sendcounts, *displs, *sendfreqcounts, *freqdispls, 
        *sendprimecounts, *sendprimecountsroot, *primedispls; // Array for how many elements to send and displacement of elements.
    int *ranArry; //array of random size.
    int *localArray, *localfreqArray, *localPrimeArray; // used to store random values for each process.
    int *localFreqTable; // Store the frequency of distink values.
    int *totalFreqTable, *totalPrimeTable, totalPrimeSize; // store sum of freq
    int not_present; //checks if the number appears in the frequency array
    int rem; // remaining elements after division among processes.
    int sum = 0; // Sum of counts. Used to calculate displacements
    int range; //Range for the random numbers
    int totalParity, sumParity;
    int localfreqZeros, TotalFreqZeros; // stores the local and total amount of zeros in the freq array.
    double startTime = 0.0, endTime;
	
    if (argc < 3)
    {
	printf("Error not enough args");
	exit(1);
    }

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
            //printf("Enter the size of the array: (0 quits) ");
            //scanf("%d", &size);
	    size = atoi(argv[1]);
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
            sendcounts = (int *)malloc(numprocs * sizeof(int));
            displs = (int *)malloc(numprocs * sizeof(int));
            sendfreqcounts = (int *)malloc(numprocs * sizeof(int));
            freqdispls = (int *)malloc(numprocs * sizeof(int));
            sendprimecounts = (int *)calloc(numprocs, sizeof(int));

            // Process 0 will create the random array and determine the size of each of the other
            // processes, include self
            if (myid == 0)
            {
                ranArry = (int *)malloc(size * sizeof(int)); // create random array
                sendprimecountsroot = (int *)calloc(numprocs, sizeof(int));

                srand(time(NULL)); //sets the random range for numbers to be gen.
                //printf("Enter Range for random numbers: ");
                //scanf("%d", &range);
		range = atoi(argv[2]);		

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
                totalFreqTable = (int *)calloc(range+1, sizeof(int));
            }

            MPI_Bcast(sendfreqcounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(freqdispls, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(sendcounts, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&range, 1, MPI_INT, 0, MPI_COMM_WORLD);
            
            localArray = (int *)malloc(sendcounts[myid] * sizeof(int));
            localFreqTable = (int *)calloc(range + 1, sizeof(int));
            localfreqArray = (int *)malloc(sendfreqcounts[myid] * sizeof(int));
            // send parts of main array to processes.
            MPI_Scatterv(ranArry, sendcounts, displs, MPI_INT, localArray, sendcounts[myid], MPI_INT, 0, MPI_COMM_WORLD);

            //*****************************************************************************
            // Find the even parity adn freq of numbers in the data
            //*****************************************************************************
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

            //*****************************************************************************
            // Find the amount of zeros in feq array and determines what numbers are prime.
            //*****************************************************************************
            localPrimeArray = (int *)malloc(sendcounts[myid] * sizeof(int));
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
                        prime += localfreqArray[i]; // increment the current processes total prime found
                        localPrimeArray[sendprimecounts[myid]] = freqdispls[myid]+i; // add the current prime number to the local array
                        sendprimecounts[myid]++; // increase the the size of the local prime number array.
                    }
                }
            }

            // Gather the needed to display the amount of primes and create an array of only prime numbers.
            MPI_Reduce(&localfreqZeros, &TotalFreqZeros, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // gives process 0 total freq table
            MPI_Reduce(&prime, &totalPrime, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // give process 0 sum of primes
            MPI_Reduce(sendprimecounts, sendprimecountsroot, numprocs, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // give process 0 sum of primes
            MPI_Reduce(&(sendprimecounts[myid]), &totalPrimeSize, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

            // Root process, 0, determine the displacments of the gathered prime numbers arrays. The
            // displacement is just the previous displace + the previous array size. 
            if (myid == 0)
            {
                primedispls = (int *)calloc(numprocs, sizeof(int));
                for (int i = 1; i < numprocs; i++)
                {
                    primedispls[i] = sendprimecountsroot[i-1] + primedispls[i-1];
                }
                totalPrimeTable = (int *)calloc(totalPrimeSize, sizeof(int));
            }
            MPI_Gatherv(localPrimeArray, sendprimecounts[myid], MPI_INT, totalPrimeTable, sendprimecountsroot, primedispls, MPI_INT, 0, MPI_COMM_WORLD);

            //MPI_Reduce(localPrimeArray, totalPrimeTable, range+1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // gives process 0 total freq table
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
                printf("||*************************************************************||\n");
                printf("||\n");
                printf("||   Total Data: %d\n", size);
                printf("||   Number of even numbers: %d\n", totalParity);
                printf("||   Total Distint Numbers: %d\n", range+1 - TotalFreqZeros);
                printf("||   Freq Table (Index = Value): ");
                PrintArray(totalFreqTable, range + 1);
                printf("||\n");
                printf("||   Total Prime Numbers: %d\n", totalPrime);
                printf("||   Total Percent of Primes: %.2f%%\n", precentPrime);
                printf("||   List of Primes: ");
                PrintArray(totalPrimeTable, totalPrimeSize);
                printf("||\n");
                printf("||   wall clock time = %f seconds\n", endTime - startTime);
                printf("||\n");
                printf("||*************************************************************||\\\n");
                printf("||                           END                               ||\n");
                printf("\\\\*************************************************************//\n\n");
            }

        }
        break; // stops from looping. May fix looping memory leak problem later.
    }

    //Free up used memory
    if (myid == 0)
    {
        free (ranArry);
        free (totalFreqTable);
        free (totalPrimeTable);
        free (sendprimecountsroot);
	free (primedispls);
    }
    free (localArray);
    free (sendcounts);
    free (displs);
    free (localFreqTable);
    free (localPrimeArray);
    free (sendprimecounts);

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

/**
 * Print the passed array.
 */
void PrintArray(int *array, int size)
{
    //printf("Array Contents: ");
    for (int i = 0; i < size-1; i++)
    {
        if (i % 15 == 0)
        {
            printf("\n");
            printf("||   ");
        }
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

    int i = 2; // start of check
    int x = num; // num divided by i
    int rem = 0; // remainder

    // Continues to check if the number is prime by starting at 2.
    //
    // eg: 23
    //      x = 23 / 2 = 11;
    //      i = 2;
    //      rem = 23 % 2 = 1;
    //      
    //  Since rem != 0, then continue to check. i will increase while x decreases until
    //  x < i. This prevents double checks for prime numbers, since 2 * 3 and 3 * 2 are the 
    //  same.
    //  
    while (i <= x)
    {
        x = num / i; 
        rem = num % i;

        if (rem == 0)
            return 0;
        i++;
    }
    return 1;
}
