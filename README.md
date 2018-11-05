# BetterThanDIalUp
This is a repository for our Cosc 420 class which is High Performance Computing. Our team name is BetterThanDialUp and our members are Lela Bones, Matthew Matuk, Preston Mouw

## Running Project 1
----------------------------
To run the project 1, cd into the file location on your computer. Then run:

``` shell
mpicc -o project1 project1.c
```

``` shell
mpiexec -n <num of processes> ./project1
```

The program will prompt you for
1. The size of a random array to generate
2. The range of numbers inside the array

Then the program will output the following:
1. Total Data
2. The number of even parody numbers
3. The even parody numbers
4. The frequency table
5. The total number of distinct numbers
6. The total number of prime numbers
7. The list of prime numbers
8. The percentage of prime numbers
9. The speed in which the program completed
