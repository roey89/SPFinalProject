/*
 * SPKDNode.h
 *
 *  Created on: 6 May 2017
 *      Author: Gal
 */

#ifndef SPKDNODE_H_
#define SPKDNODE_H_

#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPConfig.h"

typedef struct node KDTreeNode;

/**
 * initalize a kd tree with "size" points from the array of points that was received as an argument)
 */
KDTreeNode *init_tree(SPPoint** arr, int size);

/*
 * this is a private method
 * allocates enough space in memory in order to create a kd tree node
 * (this method doesn't initialize the node with values)
 */
KDTreeNode *CreateKDTreeNode();

/**
 * frees all space in memory that was taken by the tree that its root is node
 */
void DestroyKDTree(KDTreeNode *node);

/**
 * private method
 * init method for INCREMENTAL method
 */
KDTreeNode *init_sub_tree_INCREMENTAL(SPKDArray *kda, int upper_index_split);

/*
 * private method
 * init method for RANDOM method
 */
KDTreeNode *init_sub_tree_RANDOM(SPKDArray *kda);

/*
 * private method
 * init method for MAX_SPREAD method
 */
KDTreeNode *init_sub_tree_MAX_SPREAD(SPKDArray *kda);

/*
 * find the k nearest points to point t that are stored in the kd tree which its root is head.
 * the method is as described in the project document that was sent to us
 */
SPBPQueue *find_k_nearest(KDTreeNode *head, SPPoint *p, SPConfig config);

#endif /* SPKDNODE_H_ */
