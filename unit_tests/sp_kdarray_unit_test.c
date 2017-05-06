/*
 * sp_kdarray_unit_test.c
 *
 *  Created on: May 6, 2017
 *      Author: Roey
 */

/*
 * main.c
 *
 *  Created on: 28 באפר׳ 2017
 *      Author: Gal
 */
#include <stdlib.h>
#include <stdio.h>
#include "../KD_ARRAY.h"
#include "../SPPoint.h"
#include "../KD_NODE.h"


int main()
{
	SPPoint **points1 = (SPPoint **)malloc(sizeof(SPPoint *)* 5);
	double p0[] = {6.0,5.4,7.9,0.0};
	double p1[] = {6.5, 3.4, 1.0, 3.4};
	double p2[] = {7.6, 3.9, 3.45, 0.5};
	double p3[] = {33.4,37.5, 0.9, 5.5};
	double p4[] = {1.4,1.5, 12.7, 4.4};
	points1[0] = spPointCreate(p0, 4, 0);
	points1[1] = spPointCreate(p1, 4, 1);
	points1[2] = spPointCreate(p2, 4, 2);
	points1[3] = spPointCreate(p3, 4, 3);
	points1[4] = spPointCreate(p4, 4, 4);

	SPKDArray *k = Init(points1, 5);

	for(int i =0; i<5; i++)
	{
		for(int d = 0; d<4; d++)
		{
			printf("%f  ", spPointGetAxisCoor(get_P(k)[i] , d));
			fflush(NULL);
		}
		printf("\n");
		fflush(NULL);
	}
	printf("\n");
	fflush(NULL);
	printf("\n");
	fflush(NULL);
	for(int i = 0; i<4; i++)
	{
		for(int j = 0 ; j<5; j++)
		{
			printf("%d  ", get_A(k)[i][j]);
			fflush(NULL);
		}
		printf("\n");
		fflush(NULL);
	}

	printf("here \n");
	fflush(NULL);
	DKDArray *dkd = Split(k, 3);
	printf("kdl: \n");
	fflush(NULL);
	print_kd(get_kdl(dkd));

	printf("\n");
	fflush(NULL);

	printf("kdr: \n");
	fflush(NULL);
	print_kd(get_kdr(dkd));


	KDTreeNode *node = init_tree(points1, 5);
	DestroyKDTree(node);

	dkd = Split(k, 1);
		printf("kdl: \n");
		fflush(NULL);
		print_kd(get_kdl(dkd));

		printf("\n");
		fflush(NULL);

		printf("kdr: \n");
		fflush(NULL);
		print_kd(get_kdr(dkd));
	return 1;
}

