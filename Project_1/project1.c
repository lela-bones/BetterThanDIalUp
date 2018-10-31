#include <mpi.h>
#include <stdio.h>

int main(int argc, char ** argv) {
    int size, rank;
    int length;
    char name[80];
    MPI_Status status;
    int i;

    MPI_Init(&argc, &argv);
    //note that argc and argv are passed by address
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Get_processor_name(name,&length);

    if (rank ==0) {
        //server commands
        printf("Hello MPI from the server process!\n");
        for(i = 1; i < size; i++){
            MPI_Recv(name, 80, MPI_CHAR, i, 999, MPI_COMM_WORLD, &status);
            printf("Hello MPI!\n");
            printf("mesg from %d of %d on %s\n", i, size, name);
        }
    }
    else{
        //client commands
        MPI_Send(name, 80, MPI_CHAR, 0, 999, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
