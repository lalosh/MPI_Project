#include "mpi.h"
#include <stdio.h>
#include "supportingArrays.h"
#define MASTER 0
#define TAG 11

/*
mpicc mpiSingleRow.c -o mpiSingleRow;mpiexec -n 6 ./mpiSingleRow 5
dimension+1 dimension
*/
int *mpiSingleRow(int dimension, int argc, char **argv);

int main(int argc, char *argv[]) {

  int dimension;

  if(argc == 2){
     dimension = atoi(argv[1]);
  }
  else{
    fprintf(stderr, "%s\n", "no argument provided for mpiMultiCol");
    return -1;
  }

  int *result_vector = mpiSingleRow(dimension, argc, argv);

  printf("================\n");
    printf("mpiSingleRow Result: \n");
    printVector(result_vector, dimension);
  printf("================\n");
  return 0;
}



int *mpiSingleRow(int dimension, int argc, char **argv){

    int rank, tasksNumber;
    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tasksNumber);

    if(!(tasksNumber == dimension+1)){
      printf("tasks number must be one more than dimension specified\n");
      MPI_Finalize();
      exit (-1);
    }

    if( rank == MASTER ){

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

      //non-blocking send
      for (int i = 1; i < tasksNumber; i++) {

        //send one row from the input array
        MPI_Isend(
          getRow(array, dimension, dimension, i-1), /*one row needed*/
          dimension,                                /*tasksNumber of one row = dimension*/
          MPI_INT,                                  /*type of data used*/
          i,                                        /*destination*/
          TAG,                                      /*tag*/
          MPI_COMM_WORLD,                           /*communicator*/
          &request
        );

        //send the vector to multiply with
        MPI_Isend(vector, dimension, MPI_INT, i, TAG, MPI_COMM_WORLD, &request);

      }//non-blocking send

      //blocking receive
      for (int i = 1; i < tasksNumber; i++) {

        int result_value;
        MPI_Recv(&result_value, 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);

        int source = status.MPI_SOURCE;
        printf("%d FROM source: %d \n", result_value, source);

        result_vector[source-1] = result_value; //placing the result in the result_vector
      }//blocking receive

      return result_vector;
    }

    else{

      int *vec1 = createVector(dimension);
      int *vec2 = createVector(dimension);

      MPI_Recv(vec1, dimension, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);
      MPI_Recv(vec2, dimension, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);

      int result_value = vectorXvector(vec1, vec2, dimension);

      //non-blocking send
      MPI_Isend(&result_value, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &request);
    }

    MPI_Finalize();
    return NULL;
}
