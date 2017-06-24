#include "mpi.h"
#include <stdio.h>
#include "supportingArrays.h"
#define TAG 11
#define MASTER 0

/*
mpicc mpiSingleCol.c -o mpiSingleCol;mpiexec -n 6 ./mpiSingleCol 5
dimension+1 dimension
*/
int *mpiSingleCol(int dimension, int argc, char **argv);

int main(int argc, char *argv[]) {

  int dimension;

  if(argc == 2){
     dimension = atoi(argv[1]);
  }
  else{
    fprintf(stderr, "%s\n", "no argument provided for mpiMultiCol");
    return -1;
  }

  int *result_vector = mpiSingleCol(dimension, argc, argv);

  printf("================\n");
    printf("mpiSingleCol Result: \n");
    printVector(result_vector, dimension);
  printf("================\n");

  return 0;
}

int *mpiSingleCol(int dimension, int argc, char **argv){

  int tasks_count, rank;
  MPI_Request request;
  MPI_Status status;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &tasks_count);

  if(!(tasks_count == dimension+1)){
    printf("tasks_count != dimension+1\n");
    MPI_Finalize();
    exit (-1);
  }

  if(rank == MASTER){

    int rows1, cols1;
    int **array = ReadData(&rows1, &cols1, "readArray.txt");

    if(!( (rows1 == dimension) && (cols1 == dimension) )){
      fprintf(stderr, "%s\n", "*you didnt match the writen array in terms of dimension");
      return NULL;
    }

    int rows2, cols2;
    int **tmp2 = ReadData(&rows2, &cols2, "readVector.txt");
    int *vector = _2Dto1DArray(tmp2,rows2,cols2);
    if(!((rows2 == dimension)&&(cols2 == 1))){
      fprintf(stderr, "%s\n", "**you didnt match the writen array in terms of dimension");
      return NULL;
    }


    int *result_vector = createVector(dimension);
    //send
    for (int i = 1; i < tasks_count; i++) {
      MPI_Isend(
        getCol(array,dimension,dimension,(i-1)),
        dimension,
        MPI_INT,
        i,
        TAG,
        MPI_COMM_WORLD,
        &request
      );
      MPI_Isend(&vector[i-1],1,MPI_INT,i,TAG,MPI_COMM_WORLD,&request);
    }//send

    //receive
    for (int i = 1; i < tasks_count; i++) {

        int *tmp_vector = createVector(dimension);
        MPI_Recv(tmp_vector, dimension, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);

        result_vector = vectorSum(result_vector, tmp_vector, dimension);

        int source = status.MPI_SOURCE;
        printVector(tmp_vector, dimension);
        printf("is received from: %d\n", source);
        printf("--------------\n");

    }//receive

    // printVector(result_vector, dimension);
    return result_vector;
  }else{

    int *column = createVector(dimension);
    int one_value;

    MPI_Recv(column, dimension, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(&one_value, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);

    int *result_vector = vectorScalar(column, dimension, one_value);
    MPI_Isend(result_vector, dimension, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &request);
  }

  MPI_Finalize();
  return NULL;

}
