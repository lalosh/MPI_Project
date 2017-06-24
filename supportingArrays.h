/*
  This header file contain helpful methods to manipulate arrays and vectors,it's mostly
  dependent over pointers arithmetic.

  Written in C.

  Any good advice to enhance this code is very welcome, and any added code or editing
  made is appreciated.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

//create 2D array with specified rows and cols
int **create2DArray(int rows, int cols){

  //'space' will hold the total size of the 2D array
  int *space;
  space = malloc( cols * rows * sizeof(int));

  //'array' is the name of the made array
  int **array;
  array = malloc( rows * sizeof(int *));

  for (int i = 0; i < rows; i++)
    array[i] = space + (cols * i);

  //assign zeros to all elements will make sure
  //I don't have previous unwanted memory at this locations
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      array[i][j] = 0;

  return array;
}

//slicing 2D array given X and Y to refers to the position needed
//and nrows ,ncols to tell how many rows and cols to slice
int **slice_2DArray(int **array, int rows, int cols,int x,int y,int nrows,int ncols){

  if(!(x <= rows)) return NULL;
  if(!(y <= cols)) return NULL;
  if(!(x + nrows <= rows)) return NULL;
  if(!(y + ncols <= cols)) return NULL;

  int **result = create2DArray(nrows,ncols);

  int r = 0, c = 0;

  for (size_t i = x; i < (x+nrows); i++) {
    for (size_t j = y; j < (y+ncols); j++) {
        result[r][c++] = array[i][j];
    }
    c = 0;
    r++;
  }
  return result;
}

//expandRows_2DArray will add zero rows to the inputed array
int **expandRows_2DArray(int **input_array, int rows, int cols, int new_rows_to_add){

  int **array;
  array = create2DArray( (rows + new_rows_to_add), cols );

  for (int i = 0; i < rows; i++)
    for (int k = 0; k < cols; k++)
      array[i][k] = input_array[i][k];

  return array;
}

//expandCols_2DArray will add zero cols to the inputed array
int **expandCols_2DArray(int **input_array, int rows, int cols, int new_cols_to_add){

    int **array;
    array = create2DArray( rows, (cols + new_cols_to_add) );

    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        array[i][j] = input_array[i][j];

    return array;
}

//create one dimensional array or better say: "vector"
int *createVector(int dimension){

  int *space;
  space = malloc( dimension * sizeof(int));

  for (int i = 0; i < dimension; i++)
    space[i] = 0;

  return space;
}

//expandVector will add zero elements to the inputed vector
int *expandVector(int *vector, int dimension, int new_rows_to_add){

  int *result_vector = createVector( ( dimension + new_rows_to_add ) );

  for (int i = 0; i < dimension; i++)
    result_vector[i] = vector[i];

  return result_vector;
}

//print out 2D array
int print2DArray(int** array, int rows, int cols){

  if(array == NULL) {
    printf("print method received a NULL pointer\n");
    return -1;
  }

  printf("\n");

  for (int k = 0; k < rows; k++) {

    printf("[");

    for (int g = 0; g < cols; g++)
      printf("%5d", array[k][g]);

    printf("   ]\n");
  }

  printf("\n");
  return 0;
}

//will return the wanted column
int *getCol(int **array, int rows, int cols, int wantedCol){

  if( ( wantedCol > (cols-1) ) || ( wantedCol < 0 )){
    fprintf(stderr, "\n%s\n", " error at getCol method:'col wanted is beyond true dimension'" );
    return NULL;
  }

  int *ptr = createVector(rows);;

  for (int j = 0; j < rows; j++)
    ptr[j] = array[j][wantedCol];

  return ptr;
}

//will return the wanted row
int *getRow(int **array, int rows, int cols, int wantedRow){

  if( ( wantedRow > (rows-1) ) || ( wantedRow < 0 )){
    fprintf(stderr, "\n%s\n", " error at getRow method:'row wanted is beyond true dimension'" );
    return NULL;
  }

  int *ptr = createVector(cols);

  for (int j = 0; j < cols; j++)
    ptr[j] = array[wantedRow][j];

  return ptr;
}

//print out a vector
void printVector(int *vector, int dimension){

  printf("[");

  for (int t = 0; t < dimension; t++)
    printf("%5d", vector[t]);

  printf("   ] ");
  printf("\n");
}

//multiplying two vectors
int vectorXvector(int *vector1, int* vector2, int dimension){

  int result = 0;

  for (int i = 0; i < dimension; i++)
    result += vector1[i] * vector2[i];

  return result;
}

//to convert 1D array(vector) into 2D array
//note that the outputed array will has:
// rows count = ('dimension' / 'cols')
// columns count = 'cols'
int **_1Dto2DArray(int *vector, int dimension, int cols){

  int t = 0;
  int **array = create2DArray( (dimension / cols), cols);

  for (int i = 0; i < (dimension/cols); i++)
    for (int j = 0; j < cols; j++)
      array[i][j] = vector[t++];

  return array;
}

//to convert 2D array into 1D array(vector)
//not that the outputed vector will has (rows*cols) elements
int *_2Dto1DArray(int **array, int rows, int cols){

  int *vector = createVector(rows * cols);

  int t = 0;
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      vector[t++] = array[i][j];

  return vector;
}

//mutiply an array (rows*cols) with a vector(dimension)
//output will be a vector with (rows) elements
int *MatVect(int **array, int rows, int cols ,int *vector, int dimension){

  if(cols != dimension)
    return NULL;

  int *result_vector = createVector(rows);

  for (int i = 0; i < rows; i++)
    result_vector[i] = vectorXvector(getRow(array,rows,cols,i), vector, dimension);

  return result_vector;
}

//mutiply a vector by one value 'scalar'
int *vectorScalar(int *vector, int dimension, int scalar){

  for (int i = 0; i < dimension; i++)
    vector[i] = vector[i] * scalar;

  return vector;
}

//add two vectos together
int *vectorSum(int *vector1, int *vector2, int dimension){

  int *result_vector = createVector(dimension);

  for (int i = 0; i < dimension; i++)
    result_vector[i] = vector1[i] + vector2[i];

  return result_vector;
}

//place a small vector (with size absolutely smaller than where we want to place it)
void place_vector(int *result_vector, int vector_size, int *small_vector, int size, int position){

  if( ! ( (size + position) <= vector_size ) )
    fprintf(stderr, "%s\n", "error at place_vector method");

  int t = 0;

  for (int i = (position*size); i < ( (position * size) + size ); i++)
    result_vector[i] = small_vector[t++];

}

//slice a vector starting at 'starting_point',and ending at 'ending_point'
int *slice_vector(int *vector, int dimension, int starting_point, int ending_point){

  if(!(ending_point <= dimension)){
    fprintf(stderr, "%s\n", "error at slice_vector**");
    return NULL;
  }

  if(!(starting_point < ending_point)) {
    fprintf(stderr, "%s\n", "error at slice_vector++");
    return NULL;
  }

  int *tmp_vector = createVector(ending_point - starting_point);
  int t = 0;

  for (int i = starting_point; i < ending_point; i++)
    tmp_vector[t++] = vector[i];

  return tmp_vector;
}

//will drop elements from the vector according to the 'drops' argument
int *drop_from_vector(int *vector, int dimension, int drops){

  int *result_vector = createVector(dimension - drops);

  for (int i = 0; i < (dimension - drops); i++)
    result_vector[i] = vector[i];

  return result_vector;
}

//return a vector(contain random numbers) with length equal to dimension
int *RandomGenerator_1D(int dimension){

  //linear congruential generator
  printf("%s\n", "please wait a moment to finish generating random numbers");

  int *vector = createVector(dimension);

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  int seed = timeinfo -> tm_sec;

  int a = 13.2;
  int b = 21.5;
  int m = 32.7;

  int x0 = 23.236  + seed;
  int output = x0;

  for (int i = 0; i < dimension; i++)
    vector[i] = output = ((a * output) + b) % m;

  sleep(2);
  return vector;
}

//return an array(contain random numbers)
int **RandomGenerator_2D(int rows, int cols){

  //linear congruential generator
  printf("%s\n", "please wait a moment to finish generating random numbers");

  int **array = create2DArray(rows, cols);

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  int seed = timeinfo -> tm_sec;

  int a = 13.2;
  int b = 21.5;
  int m = 32.7;

  int x0 = 23.236  + seed;
  int output = x0;

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      array[i][j] = output = ((a * output) + b) % m;


  sleep(2);
  return array;
}

//print 2D array to a file name
void WriteData(int **array, int rows, int cols, char *fileName){

  if(array == NULL) {
    printf("print method received a NULL pointer\n");
  }

  FILE *fileDescriptor = fopen(fileName, "w");

  fprintf(fileDescriptor, "%d %d\n", rows, cols);

  for (int k = 0; k < rows; k++) {

    for (int g = 0; g < cols; g++)
      fprintf(fileDescriptor, "%5d", array[k][g]);

    fprintf(fileDescriptor, "\n");
  }

  fclose(fileDescriptor);
}

//print 1D array to a file name
void WriteData_1D(int *array, int rows, char *fileName){

  if(array == NULL) {
    printf("print method received a NULL pointer\n");
  }

  FILE *fileDescriptor = fopen(fileName, "w");

  fprintf(fileDescriptor, "%d 1\n", rows);

  for (int k = 0; k < rows; k++)
    fprintf(fileDescriptor, "%5d", array[k]);

  fprintf(fileDescriptor, "\n");

  fclose(fileDescriptor);
}

//read array elements from file name
int **ReadData(int *nrows, int *ncols, char *fileName){

  FILE *fileDescriptor = fopen(fileName, "r");

  int n;
  int count = -1;
  int rows,cols;
  int *vector;

  while(fscanf(fileDescriptor,"%d",&n) != EOF){

    count++;

    if(count == 0)  {
      rows = n;
      continue;
    }
    if(count == 1)  {
      cols = n;
      vector=createVector(rows*cols);
      continue;
    }

    vector[count-2] = n;
  }

  int **array = _1Dto2DArray(vector,rows*cols, cols);

  *nrows = rows;
  *ncols = cols;

  return array;
}

int ReadOneInt(char *fileName){

  FILE *fileDescriptor = fopen(fileName, "r");

  int n;
  fscanf(fileDescriptor,"%d",&n);
  // rewind(fileDescriptor);
  fclose(fileDescriptor);

  return n;
}

void WriteOneInt(int n, char *fileName){

  FILE *fileDescriptor = fopen(fileName, "w");

  fprintf(fileDescriptor, "%d", n);

  fclose(fileDescriptor);
}

//returns how many rows or cols needs to be added
//it depends on the operation of numerator approximation
int numerator_approximation(int current_rows_count, int tasks_count){
  // eg. 5/4 = 1.25(float) numerator need to be incremented to 8/4 = 2(integer)
  // eg. 3/2 = 1.50(float) numerator need to be incremented to 4/2 = 2(integer)

  float f_size =(current_rows_count / (float)tasks_count);

  int new_rows_count = current_rows_count;
  while( !(f_size == (int)f_size) )
    f_size = ( (++new_rows_count) / (float)tasks_count);

  int size = (int) f_size;
  return (new_rows_count - current_rows_count);
}
