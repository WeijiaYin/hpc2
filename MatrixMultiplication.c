// MatrixMultiplication.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <mpi.h>

int countLine(char *inputFile)
{
	FILE * fp = NULL;
	int c, lc = 0;
	int line = 0;

	fp = fopen(inputFile, "r+");
	if (fp == NULL)
	{
		printf("There is no file:%s", inputFile);
		exit(1);
	}
	while ((c = fgetc(fp)) != EOF)
	{
		if (c == '\n') line++;
		lc = c;
	}
	if (lc != '\n')
	{
		line++;
		fprintf(fp, "%c", '\n');
	}
	fclose(fp);
	return line;
}

int* readMatrixAFromFile(char *inputFile, int p, int lineNum) {
	FILE *f = NULL;
	int** matrix;
	int i = 0, j = 0, block = 0, k = 0, t = 0;
	char *result;
	char line[1024];
	int* matrixre;

	matrix = (int **)malloc(lineNum * sizeof(int *));
	for (i = 0; i < lineNum; i++)
	{
		matrix[i] = (int *)malloc(lineNum * sizeof(int));
	}
	matrixre = (int *)malloc(lineNum * lineNum * sizeof(int));
	f = fopen(inputFile, "r+");
	for (i = 0; i < lineNum; i++)
	{
		fgets(line, sizeof(line), f);
		result = strtok(line, ",");
		matrix[i][0] = atoi(result);
		for (j = 1; j < lineNum; j++) {
			result = strtok(NULL, ",");
			matrix[i][j] = atoi(result);
		}
	}
	block = lineNum / p;
	for (i = 0; i < p; i++)
	{
		for (j = 0; j < lineNum; j++)
		{
			for (k = i * block; k < i * block + block; k++)
			{
				matrixre[t] = matrix[j][k];
				t++;
			}
		}
	}
//	for (i = 0; i < lineNum; i++)
//	{
//		free(matrix[i]);
//	}
//	free(matrix);
	return matrixre;
}

int** readMatrixBFromFile(char *inputFile, int lineNum) {
	FILE *f = NULL;
	int** matrix;
	int i = 0, j = 0;
	char *result;
	char line[1024];

	matrix = (int **)malloc(lineNum * sizeof(int *));
	for (i = 0; i < lineNum; i++)
	{
		matrix[i] = (int *)malloc(lineNum * sizeof(int));
	}

	f = fopen(inputFile, "r+");
	for (i = 0; i < lineNum; i++)
	{
		fgets(line, sizeof(line), f);
		result = strtok(line, ",");
		matrix[i][0] = atoi(result);
		for (j = 1; j < lineNum; j++) {
			result = strtok(NULL, ",");
			matrix[i][j] = atoi(result);
		}
	}
	fclose(f);
	return matrix;
}

int **getSubB(int pNum, int p, int lineNum, int time, int **matrixB)
{
	int **matrix = NULL;
	int i = 0 ,block = 0, j = 0, k = 0, t = 0, q= 0;

	block = lineNum / p;
	matrix = (int **)malloc(block * sizeof(int *));
	for (i = 0; i < block; i++)
	{
		matrix[i] = (int *)malloc(block * sizeof(int));
	}
	for (j = ((time + pNum) % p) * block; j <((time + pNum) % p) * block + block; j++)
	{
		for (k = pNum*block; k < pNum*block + block; k++)
		{
			matrix[t][q] = matrixB[j][k];
			q++;
		}
		q = 0;
		t++;
	}
	return matrix;
}

int *submatrixBs(int p, int lineNum, int time, int **matrixB)
{
	int block = 0, i = 0, j = 0, k = 0, t = 0;
	int **temp = NULL;
	int *ret = NULL;

	ret = (int *)malloc(block * lineNum * sizeof(int));
	block = lineNum / p;
	for (i = 0; i < p; i++)
	{
		temp = getSubB(i, p, lineNum, time, matrixB);
		for (j = 0; j < block; j++) {
			for (k = 0; k < block; k++) {
				ret[t] = temp[j][k];
				t++;
			}
		}
	}
//	for (i = 0; i < block; i++)
//	{
//		free(temp[i]);
//	}
//	free(temp);
	return ret;
}

int *multiple(int *subA, int *subB, int p, int lineNum)
{
	int **c = NULL;
	int **a = NULL;
	int **b = NULL;
	int *cc = NULL;
	int t = 0, i = 0, j = 0, k = 0;
	int block = 0;
	int ret = 0;

	block = lineNum / p;
	a = (int **)malloc(lineNum * sizeof(int *));
	for (i = 0; i < lineNum; i++) {
		a[i] = (int *)malloc(block * sizeof(int));
	}

	b = (int **)malloc(block * sizeof(int *));
	for (i = 0; i < block; i++) {
		b[i] = (int *)malloc(block * sizeof(int));
	}

	c = (int **)malloc(lineNum * sizeof(int *));
	for (i = 0; i < lineNum; i++) {
		c[i] = (int *)malloc(block * sizeof(int));
	}

	for (i = 0; i < lineNum; i++)
	{
		for (j = 0; j < block; j++)
		{
			a[i][j] = subA[t];
			t++;
		}
	}

	t = 0;
	for (i = 0; i < block; i++) {
		for (j = 0; j < block; j++) {
			b[i][j] = subB[t];
			t++;
		}
	}

	for (i = 0; i < p; i++)
	{
		for (j = i*block; j < i*block+block; j++)
		{
			for (k = 0; k < block; k++)
			{
				ret = 0;
				for (t = 0; t < block; t++) {
					ret += a[j][t] * b[t][k];
				}
				c[j][k] = ret;
			}
		}
	}
	
	cc = (int *)malloc(lineNum * block * sizeof(int));
	t = 0;
	for (i = 0; i < lineNum; i++)
	{
		for (j = 0; j < block; j++)
		{
			cc[t] = c[i][j];
			t++;
		}
	}

//	for (i = 0; i < lineNum; i++)
//	{
//		free(a[i]);
//		free(c[i]);
//	}
//	free(a);
//	free(c);
//	for (i = 0; i < block; i++)
//	{
//		free(b[i]);
//	}
//	free(b);

	return cc;
}

int *matrixAdd(int* matrix1, int *matrix2, int p, int lineNum)
{
	int i, j ,block = 0;
	int *addMatrixCal = NULL;

	block = lineNum / p;
	addMatrixCal = (int *)malloc(lineNum * block * sizeof(int));
	for (i = 0; i < lineNum * block; i++)
	{
		addMatrixCal[i] = matrix1[i] + matrix2[i];
	}
	return addMatrixCal;
}

int main(int argc, char** argv)
{
	int *a = NULL;
	int **b = NULL;
	int *sub_matB = NULL;
	int *sub_mat_mul = NULL;
	int *subs = NULL;
	int **sub_matAs = NULL;
	int *sum = NULL;
	int **c = NULL;
	int i = 0, j = 0, q = 0, t = 0, h = 0;
	int lineNum = 0;
	int* temp;
	FILE *cf = NULL;
	MPI_Init(&argc, &argv);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (world_rank == 0)
	{
		lineNum = countLine(argv[1]);
		if (lineNum % world_size != 0 || lineNum == world_size) {
			printf("the number of processor is not correct");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		a = readMatrixAFromFile(argv[1], world_size, lineNum);
		b = readMatrixBFromFile(argv[2], lineNum);
	}
	MPI_Bcast(&lineNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf("------------------------process %d lineNum: %d\n", world_rank, lineNum);
	MPI_Bcast(&world_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf("------------------------process %d world_size: p%d\n", world_rank, world_size);
	sub_matAs = (int **)malloc(world_size * sizeof(int *));
	for (j = 0; j < world_size; j++) {
		sub_matAs[j] = (int *)malloc(lineNum * lineNum / world_size * sizeof(int));
	}
	MPI_Scatter(a, lineNum / world_size *lineNum, MPI_INT, sub_matAs[0], lineNum / world_size *lineNum, MPI_INT, 0, MPI_COMM_WORLD);

	sub_matB = (int *)malloc((lineNum * lineNum/world_size/world_size) * sizeof(int));
	if (world_rank == 0) {
		subs = submatrixBs(world_size, lineNum, 0, b);
	}
	MPI_Scatter(subs, (lineNum * lineNum / world_size / world_size), MPI_INT, sub_matB, (lineNum * lineNum / world_size / world_size), MPI_INT, 0, MPI_COMM_WORLD);

	sub_mat_mul = (int *)malloc(lineNum * lineNum / world_size * sizeof(int));
	printf("--------------------round0-----------------------------process%d\n", world_rank);
	printf("--------------------0000000-------subB------------------I am proc %d\n", world_rank);
	for (i = 0; i < (lineNum * lineNum / world_size / world_size); i++)
	{
		printf("%d ", sub_matB[i]);
	}
	printf("\n");
	printf("--------------------0000000--------subA-----------------I am proc %d\n", world_rank);
	for (i = 0; i < lineNum * lineNum / world_size; i++)
	{
		printf("%d ", sub_matAs[0][i]);
	}
	printf("\n");

	sub_mat_mul = multiple(sub_matAs[0], sub_matB, world_size, lineNum);
	printf("--------------------000000--------subMultiple-----------I am proc %d\n", world_rank);
	for (i = 0; i < lineNum * lineNum / world_size; i++)
	{
		
		printf("%d ", sub_mat_mul[i]);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	printf("\n");
	printf("-------------------------------------------------------------------\n");
	
	for (q = 1; q < world_size; q++)
	{
		printf("\n");
		printf("--------------round %d--------------process %d\n", q, world_rank);
		if (world_rank == 0)
		{
			subs = submatrixBs(world_size, lineNum, q, b);
		}
		MPI_Scatter(subs, (lineNum * lineNum / world_size / world_size), MPI_INT, sub_matB, (lineNum * lineNum / world_size / world_size), MPI_INT, 0, MPI_COMM_WORLD);

		printf("--------------------------subB----------------process%d\n", world_rank);
		for (i = 0; i < (lineNum * lineNum / world_size / world_size); i++)
		{
			printf("%d ", sub_matB[i]);
		}
		printf("\n");
		printf("--------------------------subA----------------process%d\n", world_rank);
		for (i = 0; i < lineNum * lineNum / world_size; i++)
		{
			printf("%d ", sub_matAs[q -1][i]);
		}

		if (world_rank == 0) {
			MPI_Sendrecv(sub_matAs[q -1], lineNum * lineNum / world_size, MPI_INT, world_size - 1, 0, sub_matAs[q], lineNum * lineNum / world_size, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if (world_rank == world_size - 1) {
			MPI_Sendrecv(sub_matAs[q-1], lineNum * lineNum / world_size, MPI_INT, world_rank - 1, 0, sub_matAs[q], lineNum * lineNum / world_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else {
			MPI_Sendrecv(sub_matAs[q-1], lineNum * lineNum / world_size, MPI_INT, world_rank - 1, 0, sub_matAs[q], lineNum * lineNum / world_size, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		printf("\n");
		printf("submatA------------------------------------------------------process%d\n", world_rank);
		for (i = 0; i < lineNum * lineNum / world_size; i++)
		{
			printf("%d ", sub_matAs[q-1][i]);
		}
		printf("\n");
		sub_mat_mul = matrixAdd(sub_mat_mul, multiple(sub_matAs[q], sub_matB, world_size, lineNum), world_size, lineNum);
		printf("--------------------matrixMulti-------------------------------process%d\n", world_rank);
		for (i = 0; i < lineNum * lineNum / world_size; i++)
		{
			printf("%d ", sub_mat_mul[i]);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	printf("\n");
	MPI_Barrier(MPI_COMM_WORLD);
	sum = (int *)malloc(lineNum * lineNum * sizeof(int));
	MPI_Gather(sub_mat_mul, lineNum * lineNum / world_size, MPI_INT, sum, lineNum * lineNum / world_size, MPI_INT, 0, MPI_COMM_WORLD);
	if (world_rank == 0) {
		temp = (int *)malloc(lineNum * lineNum * sizeof(int));
		printf("---------------------------------------outcome--------------------------------------------\n");
		for (i = 0; i < lineNum * lineNum; i++)
		{
			temp[i] = sum[i];
			printf("%d ", sum[i]);
		}
		c = (int **)malloc(lineNum * sizeof(int *));
		for (i = 0; i < lineNum; i++)
		{
			c[i] = (int *)malloc(lineNum * sizeof(int));
		}

		h = 0;

		for (i = 0; i < world_size; i++)
		{
			for (j = 0; j < lineNum; j++)
			{
				for (t = i * (lineNum / world_size); t < (lineNum / world_size)*i + (lineNum / world_size); t++)
				{
					c[j][t] = temp[h];
		//			printf("%d,%d,%d,%d\n",j,t,q,sum[q]);
					h++;
				}
			}
		}
		printf("\n");
		printf("--------------------final outcome-------------------------\n");
		for (i = 0; i < lineNum; i++)
		{
			for (j = 0; j < lineNum; j++)
			{
				printf("%d,", c[i][j]);
			}
			printf("\n");
		}
//		fclose(cf);
//		for (i = 0; i < lineNum; i++)
//		{
//			free(c[i]);
//		}
//		free(c);
	}
//	free(a);
//	free(b);
//	free(sub_matB);
//	free(sub_mat_mul);
//	free(subs);
//	free(sub_matAs);
	MPI_Finalize();
    return 0;
}
