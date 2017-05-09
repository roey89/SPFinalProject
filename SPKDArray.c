/*
 * SPKDArray.c
 *
 *  Created on: 6 May 2017
 *      Author: Gal
 */


#include <stdlib.h>
#include <stdio.h>
#include "SPPoint.h"
static SPPoint **p;
static int comp_index = 0;
struct KDA
{
	SPPoint **P;
	int **A;
	int size;
	int dim;
};

void print_point(SPPoint *p);

typedef struct KDA SPKDArray;

void DestroyKDArray(SPKDArray *kda);


struct DKDA
{
	SPKDArray *kdl;
	SPKDArray *kdr;
};

typedef struct DKDA DKDArray;

int KDArrayGetDim(SPKDArray *kda)
{
	return spPointGetDimension((*kda).P[0]);
	//return (*kda).dim;
}

SPPoint **get_P(SPKDArray *kd)
{
	return (*kd).P;
}

SPKDArray *CreateKDArray(int size, int dim) // add logger!!!!!!!
{
	SPKDArray *kda = (SPKDArray *)malloc(sizeof(SPKDArray));
	if(kda==NULL)
	{
		//logger
		return NULL;
	}
	(*kda).size = size;
	(*kda).P = (SPPoint **)malloc(sizeof(SPPoint *)*size);
	(*kda).A = (int **)malloc(sizeof(int *)*dim);
	if((*kda).P == NULL || (*kda).A == NULL)
	{
		//logger
		DestroyKDArray(kda);
		return NULL;
	}
	for(int i=0; i<dim; i++)
	{
		(*kda).A[i] = (int *)malloc(sizeof(int)*size);
		if((*kda).A[i] == NULL)
		{
			//logger
			DestroyKDArray(kda);
			return NULL;
		}
	}
	return kda;
}

void DestroyKDArray(SPKDArray *kda)
{
	if(kda==NULL)
	{
		return;
	}
	for(int i =0; i<(*kda).size; i++)
	{
		spPointDestroy((*kda).P[i]);
	}
	int dim = KDArrayGetDim(kda);
	if((*kda).A != NULL)
	{
		for(int i =0 ; i<dim; i++)
		{
			free((*kda).A[i]);
		}
		free((*kda).A);
	}
}


int ** get_A(SPKDArray *kd)
{
	return (*kd).A;
}

int get_KDA_Size(SPKDArray *kda)
{
	return (*kda).size;
}

int point_comp(const void *p1_index, const void *p2_index)
{
	SPPoint *p1 = p[*((int *)p1_index)];
	SPPoint *p2 = p[*((int *)p2_index)];


	if(spPointGetAxisCoor(p1,comp_index) > spPointGetAxisCoor(p2,comp_index))
	{
		return 1;
	}
	if(spPointGetAxisCoor(p1,comp_index) < spPointGetAxisCoor(p2,comp_index))
	{
		return -1;
	}

	return 0;
}

SPKDArray *Init(SPPoint** arr, int size)// add failed memory allocation
{
	SPKDArray *kd = CreateKDArray(size, spPointGetDimension(arr[0]));
	if(kd == NULL)
	{
		//logger
		return NULL;
	}
	p = (*kd).P;
	for(int i = 0; i<size; i++)
	{
		(*kd).P[i] = spPointCopy(arr[i]); /// may cause problems, maybe (*arr)[i] ~~ not
	}
	int dim =  KDArrayGetDim(kd);
	for(int i=0; i<dim; i++)
	{
		for(int j = 0; j<size; j++)
		{
			(*kd).A[i][j] = j;
		}
		qsort(((*kd).A[i]), size, sizeof(int), point_comp);
		comp_index++;
	}
	comp_index = 0;
	return kd;
}


DKDArray *Split(SPKDArray *kd, int coor) // maybe free kd memory allocation
{
	DKDArray *dkd = (DKDArray *)malloc(sizeof(DKDArray));
	int size = (*kd).size;
	int dim = KDArrayGetDim(kd);
	int left[size];
	for(int i = 0; i<size/2 + size%2; i++)
	{
		left[(*kd).A[coor][i]] = 1;
	}
	for(int i = size/2 + size%2; i<size; i++)// may not be necessary
	{
		left[(*kd).A[coor][i]] = 0;
	}

	(*dkd).kdl = CreateKDArray(size/2 + size%2, dim);
	(*dkd).kdr = CreateKDArray(size - size/2 - size%2, dim);

	if((*dkd).kdl == NULL || (*dkd).kdr == NULL )
	{
		//logger
		return NULL;
	}

	SPKDArray *kdl = (*dkd).kdl;
	SPKDArray *kdr = (*dkd).kdr;

	(*kdl).size = size/2 + size%2;
	(*kdr).size = size - size/2 - size%2;

	int kdl_p_index = 0;
	int kdr_p_index = 0;

	int *map = (int *)malloc(sizeof(int)*size);
	if(map == NULL)
	{
		DestroyKDArray(kdl);
		DestroyKDArray(kdr);
		free(dkd);
		//logger
		return NULL;
	}

	for(int i = 0; i<size; i++)
	{
		if(left[i] == 1)
		{
			(*kdl).P[kdl_p_index] = spPointCopy((*kd).P[i]);
			map[i] = kdl_p_index;
			kdl_p_index++;
		}
		else
		{
			(*kdr).P[kdr_p_index] = spPointCopy((*kd).P[i]);
			map[i] = kdr_p_index;
			kdr_p_index++;
		}
	}

	int kdl_index = 0;
	int kdr_index = 0;
	for(int i=0; i<dim; i++)
	{
		kdr_index = 0;
		kdl_index = 0;
		for(int j = 0; j<size; j++)
		{
			if(left[(*kd).A[i][j]] == 1)
			{
				(*kdl).A[i][kdl_index] = map[(*kd).A[i][j]];
				kdl_index++;
			}
			else
			{
				(*kdr).A[i][kdr_index] = map[(*kd).A[i][j]];
				kdr_index++;
			}
		}
	}
	DestroyKDArray(kd);
	return dkd;
}

void print_kd(SPKDArray *kd)
{
	int size = (*kd).size;
	int dim = spPointGetDimension((*kd).P[0]);
	printf("points in the kd array: \n");
	fflush(NULL);
	for(int i =0 ; i<size; i++)
	{
		print_point((*kd).P[i]);
	}

	printf("\n");
	fflush(NULL);

	printf("A: \n");
	fflush(NULL);
	for(int i= 0; i< dim; i++)
	{
		for(int j = 0; j<size; j++)
		{
			printf("%d  ", (*kd).A[i][j]);
			fflush(NULL);
		}
		printf("\n");
		fflush(NULL);
	}
}

SPKDArray *get_kdr(DKDArray *dkd)
{
	return (*dkd).kdr;
}

SPKDArray *get_kdl(DKDArray *dkd)
{
	return (*dkd).kdl;
}

void print_point(SPPoint *p)
{
	int dim = spPointGetDimension(p);
	for(int i =0 ; i<dim; i++)
	{
		printf("%f  ", spPointGetAxisCoor(p,i));
		fflush(NULL);
	}
	printf("\n");
	fflush(NULL);
}



