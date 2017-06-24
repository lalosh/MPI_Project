#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "supportingArrays.h"
#include <time.h>

int main(int argc, char const *argv[]) {

  FILE* fileDescriptor = fopen("time_results.txt", "w");

  int dimension;
  printf("please enter dimension wanted(integer)\n");
  scanf("%d", &dimension);

  int choice;
  while(choice != 1 && choice != 2){
    printf("do you want\n1-auto generated array and vector \n");
    printf("2-load array and vector elements from .txt file?\n");
    printf("choose 1 or 2...\n");
    scanf("%d", &choice);
  }
  if(choice == 1){

    int **array = RandomGenerator_2D(dimension, dimension);
    WriteData(array, dimension, dimension, "readArray.txt");


    int **vector = RandomGenerator_2D(dimension, 1);
    WriteData(vector, dimension, 1, "readVector.txt");

  }
  if(choice == 2){

    int rows1, rows2, cols1, cols2;

    int **array = ReadData(&rows1, &cols1, "readArray.txt");
    int **vector = ReadData(&rows2, &cols2, "readVector.txt");

    if(!((rows1 == rows2*cols2)&&(rows1 == dimension))){
      printf("you didnt write proper sized array or vector in your text files\n");

      return -1;
    }
  }

  int rows1, cols1;
  int **array = ReadData(&rows1, &cols1, "readArray.txt");
  printf("Array:\n");
  print2DArray(array, rows1, cols1);

  int rows2, cols2;
  int **tmp2 = ReadData(&rows2, &cols2, "readVector.txt");
  int *vector = _2Dto1DArray(tmp2,rows2,cols2);
  printf("Vector: \n");
  printVector(vector, rows2*cols2);

  choice = 0;
  int choice2 = 0;

  while(choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5){
    printf("choose 2 between these 5 procedure to multiply:\n");
    printf("1-mpi single row\n");
    printf("2-mpi single col\n");
    printf("3-mpi multi row\n");
    printf("4-mpi multi col\n");
    printf("5-no mpi(use matVect(serial multiplication))\n");
    scanf("%d", &choice);
  }

  choice2 = choice;
  choice = 0;

  while(choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5){
    printf("choose 1 between these 5 procedure to multiply:\n");
    printf("1-mpi single row\n");
    printf("2-mpi single col\n");
    printf("3-mpi multi row\n");
    printf("4-mpi multi col\n");
    printf("5-no mpi(use matVect(serial multiplication))\n");
    scanf("%d", &choice);
    if(choice == choice2)
      choice = 0;
  }


////////////////
if(choice == 1 || choice2 == 1){

  char str[100];
  sprintf(str, "mpicc mpiSingleRow.c -o mpiSingleRow;mpiexec -n %d ./mpiSingleRow %d", dimension+1, dimension);

  clock_t begin_mpiSingleRow = clock();

  int mpiSingleRowStatus = system(str);

  clock_t end_mpiSingleRow = clock();

  double time_spent_mpiSingleRow = (double)(end_mpiSingleRow - begin_mpiSingleRow) / CLOCKS_PER_SEC;

  fprintf(fileDescriptor,"time_spent_mpiSingleRow: %7f\n\n", time_spent_mpiSingleRow);
  printf("time_spent_mpiSingleRow: %7f\n\n", time_spent_mpiSingleRow);

}
////////////////
if(choice == 2 || choice2 == 2){

  char str[100];
  sprintf(str, "mpicc mpiSingleCol.c -o mpiSingleCol;mpiexec -n %d ./mpiSingleCol %d", dimension+1, dimension);

  clock_t begin_mpiSingleCol = clock();

  int mpiSingleColStatus = system(str);

  clock_t end_mpiSingleCol = clock();

  double time_spent_mpiSingleCol = (double)(end_mpiSingleCol - begin_mpiSingleCol) / CLOCKS_PER_SEC;

  fprintf(fileDescriptor,"time_spent_mpiSingleCol: %7f\n\n", time_spent_mpiSingleCol);
  printf("time_spent_mpiSingleCol: %7f\n\n", time_spent_mpiSingleCol);

}
////////////////

if(choice == 3 || choice2 == 3){

  printf("insert children count to to work together on mpiMultiRow\n");
  int children_count = -1;
  while( children_count <= 0 ){
    scanf("%d", &children_count);
  }

  char str[100];
  sprintf(str, "mpicc mpiMultiRow.c -o mpiMultiRow;mpiexec -n %d ./mpiMultiRow %d %d", children_count+1, dimension, children_count);

  clock_t begin_mpiMultiRow = clock();

  int mpiMultiRowStatus = system(str);

  clock_t end_mpiMultiRow = clock();

  double time_spent_mpiMultiRow = (double)(end_mpiMultiRow - begin_mpiMultiRow) / CLOCKS_PER_SEC;

  fprintf(fileDescriptor,"time_spent_mpiMultiRow:  %7f\n\n", time_spent_mpiMultiRow);
  printf("time_spent_mpiMultiRow:  %7f\n\n", time_spent_mpiMultiRow);

}
////////////////

if(choice == 4 || choice2 == 4){

  int children_count = -1;
  while( children_count <= 0 ){
    printf("insert children count to to work together on mpiMultiCol\n");
    scanf("%d", &children_count);
  }

  char str[100];
  sprintf(str, "mpicc mpiMultiCol.c -o mpiMultiCol;mpiexec -n %d ./mpiMultiCol %d %d", children_count+1, dimension, children_count);

  clock_t begin_mpiMultiCol = clock();

  int mpiMultiColStatus = system(str);

  clock_t end_mpiMultiCol = clock();

  double time_spent_mpiMultiCol = (double)(end_mpiMultiCol - begin_mpiMultiCol) / CLOCKS_PER_SEC;

  fprintf(fileDescriptor,"time_spent_mpiMultiCol:  %7f\n\n", time_spent_mpiMultiCol);
  printf("time_spent_mpiMultiCol:  %7f\n\n", time_spent_mpiMultiCol);

}

////////////////

if(choice == 5 || choice2 == 5){

  clock_t begin_matVect = clock();

  int matVectStatus =
  system("cc matVect.c -o matVect;./matVect");

  clock_t end_matVect = clock();

  double time_spent_matVect = (double)(end_matVect - begin_matVect) / CLOCKS_PER_SEC;

  fprintf(fileDescriptor,"time_spent_matVect:      %7f\n\n", time_spent_matVect);
  printf("time_spent_matVect:      %7f\n\n", time_spent_matVect);

}
////////////////

  fclose(fileDescriptor);
  printf("\ntime results are saved at time_results.txt file\n");
  return 0;
}
