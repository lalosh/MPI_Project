#include "mpi.h"
#include <stdio.h>
#include "supportingArrays.h"
#define TAG 11
#define MASTER 0

/*
mpicc mpiMultiCol.c -o mpiMultiCol;mpiexec -n 4 ./mpiMultiCol 5 3
childrencout+1 dimension childrencount
*/
int *mpiMultiCol(int dimension, int children_count,int argc, char** argv);

int main(int argc, char  *argv[]) {

  int dimension,children_count;

  if(argc == 3){
     children_count = atoi(argv[2]);
    dimension = atoi(argv[1]);
  }
  else{
    fprintf(stderr, "%s\n", "no argument provided for mpiMultiCol");
    return -1;
  }

  int *result_vector = mpiMultiCol(dimension, children_count, argc, argv);

  printf("================\n");
    printf("mpiMultiCol Result: \n");
    printVector(result_vector,dimension);
  printf("================\n");

  return 0;
}


int *mpiMultiCol(int dimension, int children_count,int argc, char** argv){

  int tasks_count, rank;
  MPI_Status status;
  MPI_Request request;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &tasks_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(!(tasks_count == children_count+1)){
    printf("tasks_count != children_count+1\n");
    MPI_Finalize();
    exit (-1);
  }

  int added_cols = numerator_approximation(dimension, children_count);
  int new_cols = dimension + added_cols;
  int size = new_cols / children_count;

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

    int **new_array = expandCols_2DArray(
                                      array,
                                      dimension,
                                      dimension,
                                      added_cols
                                    );

  //send
   for (int i = 1; i < (children_count+1); i++) {
     int **sliced = slice_2DArray(
        new_array,
        dimension,
        new_cols,
        0,
        (i-1)*size,
        dimension,
        size
      );

      int *sliced_vector = _2Dto1DArray(sliced,dimension,size);

      int *vec1 = expandVector(vector,dimension,added_cols);//shold be before the for loop
      int *vec = slice_vector(vec1,dimension+added_cols,((i-1)*size),(((i-1)*size)+size));

      MPI_Isend(sliced_vector, (dimension*size), MPI_INT, i, TAG, MPI_COMM_WORLD, &request);
      MPI_Isend(vec, size, MPI_INT, i, TAG, MPI_COMM_WORLD, &request);

   }//send

   //receive
   for (int i = 1; i < (children_count+1); i++) {
     int *tmp_vector = createVector(dimension);
     MPI_Recv(tmp_vector, dimension, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
     int source = status.MPI_SOURCE;
     printf("-------\n");
     printVector(tmp_vector,dimension);
     printf("is received from: %d\n", source);
     result_vector = vectorSum(result_vector, tmp_vector, dimension);
   }//receive
  //  printVector(result_vector, dimension);
   return result_vector;

  }else{
    int *sliced_vector = createVector(size * dimension);
    int *vec = createVector(size);

    MPI_Recv(sliced_vector, (size*dimension), MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(vec, size, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &status);

    int **_2DArray = _1Dto2DArray(
        sliced_vector,
        ( size * dimension ),
        size
      );

    int *result_vector = MatVect(
        _2DArray,
        dimension,
        size,
        vec,
        size
      );
    MPI_Isend(result_vector, dimension, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &request);
  }


  MPI_Finalize();
  return NULL;
}
