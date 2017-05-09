/*
 * sp_kdarray_unit_test.c
 *
 *  Created on: 9 May 2017
 *      Author: Gal
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "unit_test_util.h"
#include "../SPPoint.h"
#include "../SPBPriorityQueue.h"
#include "../SPKDArray.h"
#include "../SPKDNode.h"

void Create_N_Destroy_Test() {
	//todo
}

bool equal_arrays(int *arr1, int *arr2, int size) {
	bool res = true;
	for (int i = 0; i < size; i++) {
		if (arr1[i] != arr2[i]) {
			res = false;
		}
	}
	return res;
}

void print_mat(int **a, int n, int m) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf("%d  ", a[i][j]);
		}
		printf(" \n");
		fflush(NULL);
	}
}

bool Init_Test() {
	SPPoint **points1 = (SPPoint **) malloc(sizeof(SPPoint *) * 5);
	double p0[] = { 6.0, 5.4, 7.9, 0.0 };
	double p1[] = { 6.5, 3.4, 1.0, 3.4 };
	double p2[] = { 7.6, 3.9, 3.45, 0.5 };
	double p3[] = { 33.4, 37.5, 0.9, 5.5 };
	double p4[] = { 1.4, 1.5, 12.7, 4.4 };
	points1[0] = spPointCreate(p0, 4, 0);
	points1[1] = spPointCreate(p1, 4, 1);
	points1[2] = spPointCreate(p2, 4, 2);
	points1[3] = spPointCreate(p3, 4, 3);
	points1[4] = spPointCreate(p4, 4, 4);

	SPKDArray *kda = Init(points1, 5);
	DestroyKDArray(kda);

	int **A = get_A(kda);
	int check_res[4][5] = { { 4, 0, 1, 2, 3 }, { 4, 1, 2, 0, 3 }, { 3, 1, 2, 0,
			4 }, { 0, 2, 1, 4, 3 } };
	for (int i = 0; i < 4; i++) {
		ASSERT_TRUE(equal_arrays(A[i], check_res[i], 5));
	}
	for (int i = 0; i < 5; i++) {
		free(points1[i]);
	}
	free(points1);
	return true;
}

bool Split_Test() {
	SPPoint **points1 = (SPPoint **) malloc(sizeof(SPPoint *) * 5);
	double p0[] = { 6.0, 5.4, 7.9, 0.0 };
	double p1[] = { 6.5, 3.4, 1.0, 3.4 };
	double p2[] = { 7.6, 3.9, 3.45, 0.5 };
	double p3[] = { 33.4, 37.5, 0.9, 5.5 };
	double p4[] = { 1.4, 1.5, 12.7, 4.4 };
	points1[0] = spPointCreate(p0, 4, 0);
	points1[1] = spPointCreate(p1, 4, 1);
	points1[2] = spPointCreate(p2, 4, 2);
	points1[3] = spPointCreate(p3, 4, 3);
	points1[4] = spPointCreate(p4, 4, 4);

	SPKDArray *kda = Init(points1, 5);

	Split(kda, 3);
}

int main() {
	RUN_TEST(Init_Test);
	return 1;
}
