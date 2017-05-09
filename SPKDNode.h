/*
 * SPKDNode.h
 *
 *  Created on: 6 May 2017
 *      Author: Gal
 */

#ifndef SPKDNODE_H_
#define SPKDNODE_H_

#include "SPPoint.h"
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"

typedef struct node KDTreeNode;

KDTreeNode *init_tree(SPPoint** arr, int size);

KDTreeNode *CreateKDTreeNode();

void DestroyKDTree(KDTreeNode *node);

KDTreeNode *init_sub_tree_INCREMENTAL(SPKDArray *kda, int upper_index_split);

KDTreeNode *init_sub_tree_RANDOM(SPKDArray *kda);

KDTreeNode *init_sub_tree_MAX_SPREAD(SPKDArray *kda);

SPBPQueue *find_k_nearest(KDTreeNode *head, SPPoint *p);

#endif /* SPKDNODE_H_ */
