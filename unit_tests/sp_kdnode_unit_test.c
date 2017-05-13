/*
 * sp_kdnode_unit_test.c
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "../SPKDNode.h"
#include "../SPConfig.h"
#include "unit_test_util.h"

bool Create_N_Destroy_Node_Test() {
	KDTreeNode *node = CreateKDTreeNode();
	DestroyKDTree(node);
	return true;
}

bool init_sub_tree_MAX_SPREAD_Test() {
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

	KDTreeNode *node = init_tree(points1, 5);
	DestroyKDTree(node);
	spPointDestroy(points1[0]);
	spPointDestroy(points1[1]);
	spPointDestroy(points1[2]);
	spPointDestroy(points1[3]);
	spPointDestroy(points1[4]);
	free(points1);

	return true;
}

bool Find_K_Nearest() {
	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc (sizeof(SP_CONFIG_MSG));
	if (!msg) {
		//todo
	}
	SPConfig config = spConfigCreate("spcbir_kdnode_test.config", msg);
	free(msg);
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

	KDTreeNode *node = init_tree(points1, 5);
	SPBPQueue *queue = find_k_nearest(node, points1[0], config);
	spConfigDestroy(config);

	double correct_vals[3] = {0.0 , 24.8625, 63.42};
	int correct_index[3] = {0,2,1};
	int i =0;
	while (!spBPQueueIsEmpty(queue)) {
		BPQueueElement* res = (BPQueueElement *) malloc(sizeof(BPQueueElement));
		spBPQueuePeek(queue, res);
		if((abs((*res).value- correct_vals[i]) > 0.0 )|| (correct_index[i] != (*res).index ))
		{
			return false;
		}
		i++;
		spBPQueueDequeue(queue);
		free(res);

	}
	spPointDestroy(points1[0]);
	spPointDestroy(points1[1]);
	spPointDestroy(points1[2]);
	spPointDestroy(points1[3]);
	spPointDestroy(points1[4]);
	free(points1);
	DestroyKDTree(node);
	spBPQueueDestroy(queue);
	return true;
}

int main()
{
	RUN_TEST(Create_N_Destroy_Node_Test);
	RUN_TEST(init_sub_tree_MAX_SPREAD_Test);
	RUN_TEST(Find_K_Nearest);
	return 1;
}

