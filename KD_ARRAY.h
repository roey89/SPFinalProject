/*
 * KD_ARRAY.h
 *
 *  Created on: 6 May 2017
 *      Author: Gal
 */

#ifndef KD_ARRAY_H_
#define KD_ARRAY_H_

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


#endif /* KD_ARRAY_H_ */
