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

typedef struct node KDTreeNode;

KDTreeNode *init_tree(SPPoint** arr, int size);

KDTreeNode *CreateKDTreeNode();

void DestroyKDTree(KDTreeNode *node);

#endif /* SPKDNODE_H_ */
