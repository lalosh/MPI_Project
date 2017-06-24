#include "mpi.h"
#include <stdio.h>
#include "supportingArrays.h"
#define TAG 11
#define MASTER 0

/*
mpicc mpiMultiRow.c -o mpiMultiRow;mpiexec -n 4 ./mpiMultiRow 5 3
childrencout+1 dimension childrencount
*/

int *mpiMultiRow(int dimension, int children_count, int argc, char **argv);

int main(int argc, char *argv[]) {

  int dimension,children_count;

  if(argc == 3){
     children_count = atoi(argv[2]);
     dimension = atoi(argv[1]);
  }
  else{
    fprintf(stderr, "%s\n", "no argument provided for mpiMultiCol");
    return -1;
  }

  int *result_vector = mpiMultiRow(dimension, children_count, argc, argv);

  printf("================\n");
    printf("mpiMultiRow Result: \n");
    printVector(result_vector,dimension);
  printf("================\n");

  return 0;
}

int *mpiMultiRow(int dimension, int children_count, int argc, char **argv){

  int tasks_count, rank;
  MPI_Request request;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &tasks_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(!(tasks_count == children_count+1)){
    printf("tasks_count != children_count+1\n");
    MPI_Finalize();
    exit (-1);
  }

  int added_rows = numerator_approximation(dimension, children_count);
  int new_rows = dimension + added_rows;
  int size = new_rows / children_count;

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


    int **new_array = expandRows_2DArray(
                                      array,
                                      dimension,
                                      dimension,
                                      added_rows
                                    );
    int *result_vector = createVector(size * children_count);

    //send
    for (int i = 1; i < (children_count+1); i++) {
      int **sliced = slice_2DArray(
          new_array,
          new_rows,
          dimension,
          (i-1) * size,
          0,
          size,
          dimension
        );

      int *sliced_vector = _2Dto1DArray(sliced, size, dimension);
      // printVector(sliced_vector, (size*dimension));
      MPI_Isend(sliced_vector, (size*dimension), MPI_INT, i, TAG, MPI_COMM_WORLD, &request);
      // printVector(vector, dimension);
      MPI_Isend(vector, dimension, MPI_INT, i, TAG, MPI_COMM_WORLD, &request);

    }//send

    //receive
    for (int i = 1; i < (children_count+1); i++) {
      int *vec = createVector(size);
      MPI_Recv(vec, size, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
      int source = status.MPI_SOURCE;
      printVector(vec, size);
      printf("receive source: %d\n", source);
      printf("---------------------\n");
      int index = source - 1;
      place_vector(
             result_vector, size * children_count, /*result_vector along with its dimension*/
             vec, size, index /*place tmp_vector inside result_vector where position = 'index'*/
           );

    }//receive

    int *final_result_vector = drop_from_vector(
                                            result_vector, /*input vector*/
                                            (size * children_count),/*input vector dimension*/
                                            added_rows /*wanted rows to be droped*/
                                          );


    // printVector(final_result_vector,dimension);
    return final_result_vector;

  }
  else{
    int *sliced_vector = createVector(size * dimension);
    int *vector = createVector(dimension);

    MPI_Recv(sliced_vector, (size*dimension), MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(vector, dimension, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);

    int **_2DArray = _1Dto2DArray( sliced_vector, ( size * dimension ), dimension );
   //
    int *result_vector = MatVect(
                                      _2DArray,
                                      size,         /*rows of _2DArray*/
                                      dimension, /*columns of _2DArray*/
                                      vector,
                                      dimension  /*vector dimension*/
                                    );
   //
   MPI_Isend(result_vector, size, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &request);
  }


  MPI_Finalize();
  return NULL;
}
