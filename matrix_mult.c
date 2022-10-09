#include<stdlib.h>
#include<stdio.h>

#define SIZE 2048

void matrixmult_v1(int (*matrix1)[SIZE], int (*matrix2)[SIZE])
{

	int (*result)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));

	for(int i = 0; i<SIZE; i++)
		for(int j = 0; j<SIZE; j++)
			result[i][j] = 0;
	for(int i = 0; i<SIZE; i++)
	{
		for(int j = 0; j<SIZE; j++)
		{
			for(int k = 0; k<SIZE; k++)
			{
				result[i][j] += matrix1[i][k]*matrix2[k][j];
			}
		}
	}
}

void matrixmult_v2(int (*matrix1)[SIZE], int (*matrix2)[SIZE])
{

	int (*result)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));

	for(int i = 0; i<SIZE; i++)
		for(int j = 0; j<SIZE; j++)
			result[i][j] = 0;

	for(int i = 0; i<SIZE; i++)
	{
		for(int k = 0; k<SIZE; k++)
		{
			for(int j = 0; j<SIZE; j++)
			{
				result[i][j] += matrix1[i][k]*matrix2[k][j];
			}
		}
	}
}

void matrixmult_v3(int (*matrix1)[SIZE], int (*matrix2)[SIZE])
{

	int (*result)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));

	for(int i = 0; i<SIZE; i++)
		for(int j = 0; j<SIZE; j++)
			result[i][j] = 0;

	for(int k = 0; k<SIZE; k++)
	{
		for(int i = 0; i<SIZE; i++)
		{
			for(int j = 0; j<SIZE; j++)
			{
				result[i][j] += matrix1[i][k]*matrix2[k][j];
			}
		}
	}
}

int main()
{

	int (*matrix1)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));
	int (*matrix2)[SIZE] = malloc(sizeof(int[SIZE][SIZE]));

	//Initializing the matrix

	for(int i = 0; i<SIZE; i++)
		for(int j = 0; j<SIZE; j++)
		{
			matrix1[i][j] = 0;
			matrix2[i][j] = 0;
		}	

	matrixmult_v1(matrix1,matrix2);

}