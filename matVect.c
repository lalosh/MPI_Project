#include <stdio.h>
#include "supportingArrays.h"

int main(int argc, char const *argv[]) {

  int rows1, cols1;
  int **array = ReadData(&rows1, &cols1, "readArray.txt");

  int rows2, cols2;
  int **tmp2 = ReadData(&rows2, &cols2, "readVector.txt");
  int *vector = _2Dto1DArray(tmp2,rows2,cols2);

  int *result_vector = MatVect(array, rows1, cols1, vector, rows2*cols2);

  printf("=================================\n");
  printf("matVect Result: \n");
  printVector(result_vector,rows2*cols2);
  printf("=================================\n");

  return 0;
}
