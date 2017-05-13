/*
 * SPKDArray.h
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

typedef struct KDA SPKDArray;

typedef struct DKDA DKDArray;

/**
 * gets the dimension of the kd-array
 */
int KDArrayGetDim(SPKDArray *kda);


/**
 * gets the points stored in the kd-array
 */
SPPoint **get_P(SPKDArray *kd);


/**
 * destroys (free the space) that is used by the kd-array that was received as an argument
 */
void DestroyKDArray(SPKDArray *kda);


/**
 * allocates space in memory for a kd array with "size" points with dimension and dim
 */
SPKDArray *CreateKDArray(int size, int dim);


/**
 * private method, only for unit testing
 * gets the A array that is used in the description of kd array
 */
int ** get_A(SPKDArray *kd);

/**
 * returns the number of points in the kd array
 */
int get_KDA_Size(SPKDArray *kda);


/**
 * initalize a kd array with "size" points from "arr"
 */
SPKDArray *Init(SPPoint** arr, int size);

/**
 * splits the kd array (as described in the FinalProject.pdf)
 */
DKDArray *Split(SPKDArray *kd, int coor);


/**
 * gets the right side of the dkd array (they right side we get after we split the kd array)
 */
SPKDArray *get_kdr(DKDArray *dkd);


/*
 *  gets the right side of the dkd array (they right side we get after we split the kd array)
 */
SPKDArray *get_kdl(DKDArray *dkd);

void print_point(SPPoint *p);

#endif /* SPKDARRAY_H_ */
