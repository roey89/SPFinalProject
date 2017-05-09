/*
 * SPPoint.c
 *
 *  Created on: Dec 31, 2016
 *      Author: Roey
 */
#include "SPPoint.h"
#include <malloc.h>
#include <assert.h>

struct sp_point_t {
	int index;
	int dimension;
	double* data;
};

SPPoint* spPointCreate(double* data, int dim, int index) {
	if (data == NULL || dim <= 0 || index < 0) {
		return NULL;
	}

	SPPoint* point = (SPPoint*) malloc(sizeof(SPPoint)); // allocation

	if (point == NULL) {
		return NULL;
	}

	// fields
	point->data = (double*) malloc(dim * sizeof(data[0])); // allocation
	for (int i = 0; i < dim; i++) {
		(point->data)[i] = (data)[i];
	}
	point->index = index;
	point->dimension = dim;
	return point;
}

SPPoint* spPointCopy(SPPoint* source) {
	assert(source != NULL);
	SPPoint* point = (SPPoint*) malloc(sizeof(SPPoint)); // allocation
	point->index = source->index;
	point->dimension = source->dimension;
	point->data = (double*) malloc(source->dimension * sizeof(source->data[0])); // allocation
	for (int i = 0; i < source->dimension; i++) {
		(point->data)[i] = (source->data)[i];
	}
	return point;
}

void spPointDestroy(SPPoint* point) {
	assert(point != NULL);
	if (point->data != NULL) {
		free((point->data));
	}
	free(point);
}

int spPointGetDimension(SPPoint* point) {
	assert(point != NULL);
	return point->dimension;
}

int spPointGetIndex(SPPoint* point) {
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint* point, int axis) {
	assert(point != NULL);
	return (point->data[axis]);
}

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q) {
	assert(p != NULL);
	double distance = 0, pDim = spPointGetDimension(p);
	assert(q != NULL && pDim == spPointGetDimension(q));
// computing sum((p_i - q_i)^2) (for 1<=i<=d)
	for (int i = 0; i < pDim; i++) {
		distance += (spPointGetAxisCoor(p, i) - spPointGetAxisCoor(q, i))
				* (spPointGetAxisCoor(p, i) - spPointGetAxisCoor(q, i));
	}
	return distance;
}
