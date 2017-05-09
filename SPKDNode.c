/*
 * SPKDNode.C
 *
 *  Created on: 6 May 2017
 *      Author: Gal
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "SPKDNode.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"

//todo

#define spKNN 5

struct node {
	int dim;
	SPPoint *val;
	struct node *left;
	struct node *right;
	SPPoint *Data;
};

static bool KDTree_init_memory_overflow = 0;

KDTreeNode * CreateKDTreeNode() {
	KDTreeNode *node = (KDTreeNode *) malloc(sizeof(KDTreeNode));
	if (node == NULL) {
		// logger - or maybe after
		return NULL;
	}
	(*node).right = NULL;
	(*node).left = NULL;
	return node;
}

KDTreeNode *init_sub_tree_MAX_SPREAD(SPKDArray *kda);

KDTreeNode *init_sub_tree_INCREMENTAL(SPKDArray *kda, int upper_index_split);

KDTreeNode *init_sub_tree_RANDOM(SPKDArray *kda);

void kNearestNeighbors(KDTreeNode *curr, SPBPQueue *queue, SPPoint *p);

void DestroyKDTree(KDTreeNode *node) {
	if (node == NULL) {
		return;
	}
	DestroyKDTree((*node).right);
	DestroyKDTree((*node).left);
	free((*node).val);
	free((*node).Data);
	free(node);
}

KDTreeNode *init_tree(SPPoint** arr, int size) {
	KDTreeNode *node;
	SPKDArray *kda = Init(arr, size);
	if (kda == NULL) {
		//logger
		return NULL;
	}
	if (true) // MAX_SPREAD
	{
		node = init_sub_tree_MAX_SPREAD(kda);
	}
	if (false) //RANDOM
	{
		node = init_sub_tree_RANDOM(kda);
	}
	if (false) // INCREMENTAL
	{
		node = init_sub_tree_INCREMENTAL(kda, 0);
	}
	if (KDTree_init_memory_overflow) {
		DestroyKDTree(node);
		KDTree_init_memory_overflow = false;
	}
	return node;
}

KDTreeNode *init_sub_tree_MAX_SPREAD(SPKDArray *kda) {
	int size = get_KDA_Size(kda);
	KDTreeNode *node = (KDTreeNode *) malloc(sizeof(KDTreeNode));
	if (node == NULL) {
		KDTree_init_memory_overflow = true;
		//logger
		return NULL;
	}
	if (size == 1) // sheilta of getting the size if the SPKDArray
			{
		(*node).left = NULL;
		(*node).right = NULL;
		(*node).Data = spPointCopy((get_P(kda))[0]); //
		return node;
	}
	int dim = spPointGetDimension((get_P(kda))[0]); //
	int dim_index_split = -1;
	SPPoint **points = get_P(kda);
	int **A = get_A(kda);
	int max = -1;
	int min = 0;
	//int dim_index = -1;
	for (int i = 0; i < dim; i++) {
		int curr_max = spPointGetAxisCoor(points[A[i][size - 1]], i);
		int curr_min = spPointGetAxisCoor(points[A[i][0]], i);
		if (curr_max - curr_min > max - min) {
			max = curr_max;
			min = curr_min;
			dim_index_split = i;
		}
	}
	(*node).dim = dim_index_split;
	(*node).val = spPointCopy(
			get_P(kda)[A[dim_index_split][size / 2 + (size % 2)]]); // maybe just size/2
	DKDArray *dkd = Split(kda, dim_index_split);
	(*node).left = init_sub_tree_MAX_SPREAD(get_kdl(dkd));
	(*node).right = init_sub_tree_MAX_SPREAD(get_kdr(dkd));
	(*node).Data = NULL;
	return node;
}

KDTreeNode *init_sub_tree_RANDOM(SPKDArray *kda) {
	int size = get_KDA_Size(kda);
	KDTreeNode *node = (KDTreeNode *) malloc(sizeof(KDTreeNode));
	if (node == NULL) {
		KDTree_init_memory_overflow = true;
		//logger
		return NULL;
	}
	if (size == 1) // sheilta of getting the size if the SPKDArray
			{
		(*node).left = NULL;
		(*node).right = NULL;
		(*node).Data = spPointCopy((get_P(kda))[0]); //
		return node;
	}
	int dim = spPointGetDimension((get_P(kda))[0]); //
	int dim_index_split = rand() & dim;
	int **A = get_A(kda);
	(*node).dim = dim_index_split;
	(*node).val = spPointCopy(
			get_P(kda)[A[dim_index_split][size / 2 + (size % 2)]]); // maybe just size/2
	DKDArray *dkd = Split(kda, dim_index_split);
	(*node).left = init_sub_tree_RANDOM(get_kdl(dkd));
	(*node).right = init_sub_tree_RANDOM(get_kdr(dkd));
	(*node).Data = NULL;
	return node;
}

KDTreeNode *init_sub_tree_INCREMENTAL(SPKDArray *kda, int upper_index_split) {
	int size = get_KDA_Size(kda);
	KDTreeNode *node = (KDTreeNode *) malloc(sizeof(KDTreeNode));
	if (node == NULL) {
		KDTree_init_memory_overflow = true;
		//logger
		return NULL;
	}
	if (size == 1) {
		(*node).val = NULL;
		(*node).left = NULL;
		(*node).right = NULL;
		(*node).Data = spPointCopy((get_P(kda))[0]); //
		return node;
	}
	int dim = spPointGetDimension((get_P(kda))[0]); //
	int dim_index_split = (upper_index_split + 1) % dim;
	int **A = get_A(kda);
	(*node).dim = dim_index_split;
	(*node).val = spPointCopy(
			get_P(kda)[A[dim_index_split][size / 2 + (size % 2)]]); // maybe just size/2 // needs to be a point
	DKDArray *dkd = Split(kda, dim_index_split);
	(*node).left = init_sub_tree_INCREMENTAL(get_kdl(dkd), dim_index_split);
	(*node).right = init_sub_tree_INCREMENTAL(get_kdr(dkd), dim_index_split);
	(*node).Data = NULL;
	return node;
}

int isLeaf(KDTreeNode *curr) {
	if ((*curr).left == NULL && (*curr).right == NULL) {
		return 1;
	}
	return 0;
}

SPBPQueue *find_k_nearest(KDTreeNode *head, SPPoint *p) {
	SPBPQueue* queue = spBPQueueCreate(spKNN);
	if (queue == NULL) {
		return NULL;
	}
	kNearestNeighbors(head, queue, p);
	return queue;
}

//todo: init for k nearest search
void kNearestNeighbors(KDTreeNode *curr, SPBPQueue *queue, SPPoint *p) {
	if (curr == NULL) {
		return;
	}
	if (isLeaf(curr) == 1) {
		spBPQueueEnqueue(queue, spPointGetIndex((*curr).Data),
				spPointL2SquaredDistance((*curr).Data, p));
		return;
	}
	int val = spPointGetAxisCoor((*curr).val, (*curr).dim);
	KDTreeNode *other_subtree = NULL;
	if (spPointGetAxisCoor(p, (*curr).dim) <= val) {
		kNearestNeighbors((*curr).left, queue, p);
		other_subtree = (*curr).right;
	} else {
		kNearestNeighbors((*curr).right, queue, p);
		other_subtree = (*curr).left;
	}
	bool queue_is_full = spBPQueueIsFull(queue);
	double dist_priority = (val - spPointGetAxisCoor(p, (*curr).dim))
			* (val - spPointGetAxisCoor(p, (*curr).dim));

	if ((!queue_is_full) || dist_priority < spBPQueueMaxValue(queue)) {
		//todo: what does search the other subtree on the next axis means
		kNearestNeighbors(other_subtree, queue, p);
	}

}
