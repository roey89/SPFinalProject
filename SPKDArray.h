/*
 * SPKDArray.h
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

void print_point(SPPoint *p);

typedef struct KDA SPKDArray;

typedef struct DKDA DKDArray;

int KDArrayGetDim(SPKDArray *kda);

SPPoint **get_P(SPKDArray *kd);

void DestroyKDArray(SPKDArray *kda);

SPKDArray *CreateKDArray(int size, int dim);

int ** get_A(SPKDArray *kd);

int get_KDA_Size(SPKDArray *kda);

SPKDArray *Init(SPPoint** arr, int size);

DKDArray *Split(SPKDArray *kd, int coor);

void print_kd(SPKDArray *kd);

SPKDArray *get_kdr(DKDArray *dkd);

SPKDArray *get_kdl(DKDArray *dkd);

void print_point(SPPoint *p);

#endif /* SPKDARRAY_H_ */
