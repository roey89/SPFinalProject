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

void Create_N_Destroy_KDArray_Test() {
	//todo
}
/**
 void //print_point(SPPoint *p)
 {
 int dim = spPointGetDimension(p);
 for(int i =0 ;i<dim; i++)
 {
 ////printf("%f  ", spPointGetAxisCoor(p, dim));
 }
 ////printf("\n");
 //fflush(NULL);;
 }
 */
/**
 *

void //print_array(int *arr, int size) {
	for (int i = 0; i < size; i++) {
		////printf("%d  ", arr[i]);
		//fflush(NULL);;
	}
	////printf("\n");
	//fflush(NULL);;
}
*
 */
bool equal_arrays(int *arr1, int *arr2, int size) {
	bool res = true;
	for (int i = 0; i < size; i++) {
		if (arr1[i] != arr2[i]) {
			res = false;
		}
	}
	return res;
}

/**
void //print_mat(int **a, int n, int m) {
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++) {
			////printf("%d  ", a[i][j]);
		}
		////printf(" \n");
		//fflush(NULL);;
	}
}

 */

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

	SPPoint **P = get_P(kda);
	for (int i = 0; i < 5; i++) {
		ASSERT_TRUE(spPointL2SquaredDistance(P[i], points1[i]) == 0);
	}
	int **A = get_A(kda);
	int check_res[4][5] = { { 4, 0, 1, 2, 3 }, { 4, 1, 2, 0, 3 }, { 3, 1, 2, 0,
			4 }, { 0, 2, 1, 4, 3 } };
	for (int i = 0; i < 4; i++) {
		ASSERT_TRUE(equal_arrays(A[i], check_res[i], 5));
	}
	DestroyKDArray(kda);
	for (int i = 0; i < 5; i++) {
		spPointDestroy(points1[i]);
	}
	free(points1);
	return true;
}

bool Split_Test1() {
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

	DKDArray *dkd = Split(kda, 3);

	SPPoint **kdl_p_check = (SPPoint **) malloc(sizeof(SPPoint *) * 3);
	kdl_p_check[0] = points1[0];
	kdl_p_check[1] = points1[1];
	kdl_p_check[2] = points1[2];

	int kdl_res_check[4][3] = { { 0, 1, 2 }, { 1, 2, 0 }, { 1, 2, 0 },
			{ 0, 2, 1 } };

	SPPoint **kdr_p_check = (SPPoint **) malloc(sizeof(SPPoint *) * 2);
	kdr_p_check[0] = points1[3];
	kdr_p_check[1] = points1[4];

	int kdr_res_check[4][2] = { { 1, 0 }, { 1, 0 }, { 0, 1 }, { 1, 0 } };

	SPKDArray *kdl = get_kdl(dkd);
	SPKDArray *kdr = get_kdr(dkd);

	SPPoint **kdl_p = get_P(kdl);
	SPPoint **kdr_p = get_P(kdr);

	int ** kdr_A = get_A(kdr);
	int ** kdl_A = get_A(kdl);

	for (int i = 0; i < 3; i++) {
		//print_point(kdl_p[i]);
		//print_point(kdl_p_check[i]);
		ASSERT_TRUE(spPointL2SquaredDistance(kdl_p[i], kdl_p_check[i]) == 0);
	}
	for (int i = 0; i < 2; i++) {
		//print_point(kdr_p[i]);
		//print_point(kdr_p_check[i]);
		ASSERT_TRUE(spPointL2SquaredDistance(kdr_p[i], kdr_p_check[i]) == 0);
	}

	for (int i = 0; i < 4; i++) {
		//print_array(kdl_A[i], 3);
		//print_array(kdl_res_check[i], 3);
		//print_array(kdr_A[i], 2);
		//print_array(kdr_res_check[i], 2);
		ASSERT_TRUE(equal_arrays(kdl_A[i], kdl_res_check[i], 3));
		ASSERT_TRUE(equal_arrays(kdr_A[i], kdr_res_check[i], 2));
	}

	DestroyKDArray(kdl);
	DestroyKDArray(kdr);
	for (int i = 0; i < 5; i++) {
		spPointDestroy(points1[i]);
	}
	free(kdl_p_check);
	free(kdr_p_check);
	free(points1);
	return true;
}

bool Split_Test2() {
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

	DKDArray *dkd = Split(kda, 2);

	SPPoint **kdl_p_check = (SPPoint **) malloc(sizeof(SPPoint *) * 3);
	kdl_p_check[0] = points1[1];
	kdl_p_check[1] = points1[2];
	kdl_p_check[2] = points1[3];

	int kdl_res_check[4][3] = { { 0, 1, 2 }, { 0, 1, 2 }, { 2, 0, 1 },
			{ 1, 0, 2 } };

	SPPoint **kdr_p_check = (SPPoint **) malloc(sizeof(SPPoint *) * 2);
	kdr_p_check[0] = points1[0];
	kdr_p_check[1] = points1[4];

	int kdr_res_check[4][2] = { { 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 } };

	SPKDArray *kdl = get_kdl(dkd);
	SPKDArray *kdr = get_kdr(dkd);

	SPPoint **kdl_p = get_P(kdl);
	SPPoint **kdr_p = get_P(kdr);

	int ** kdr_A = get_A(kdr);
	int ** kdl_A = get_A(kdl);

	for (int i = 0; i < 3; i++) {
		//print_point(kdl_p[i]);
		//print_point(kdl_p_check[i]);
		ASSERT_TRUE(spPointL2SquaredDistance(kdl_p[i], kdl_p_check[i]) == 0);
	}
	for (int i = 0; i < 2; i++) {
		//print_point(kdr_p[i]);
		//print_point(kdr_p_check[i]);
		ASSERT_TRUE(spPointL2SquaredDistance(kdr_p[i], kdr_p_check[i]) == 0);
	}

	for (int i = 0; i < 4; i++) {
		//print_array(kdl_A[i], 3);
		//print_array(kdl_res_check[i], 3);
		//print_array(kdr_A[i], 2);
		//print_array(kdr_res_check[i], 2);
		ASSERT_TRUE(equal_arrays(kdl_A[i], kdl_res_check[i], 3));
		ASSERT_TRUE(equal_arrays(kdr_A[i], kdr_res_check[i], 2));
	}

	DestroyKDArray(kdl);
	DestroyKDArray(kdr);
	for (int i = 0; i < 5; i++) {
		spPointDestroy(points1[i]);
	}
	free(kdl_p_check);
	free(kdr_p_check);
	free(points1);
	return true;
}

int main()
{
	RUN_TEST(Init_Test);
	RUN_TEST(Split_Test1);
	RUN_TEST(Split_Test2);
	return 1;
}
