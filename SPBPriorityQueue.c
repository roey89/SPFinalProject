/*
 * SPBPriorityQueue.c
 *
 *  Created on: Jan 1, 2017
 *      Author: Roey
 */
#include "SPBPriorityQueue.h"
#include <malloc.h>
#include <stdio.h>

struct sp_bp_queue_t {
	int size;
	BPQueueElement** elements;
	int maxSize;
};

SPBPQueue* spBPQueueCreate(int maxSize) {
	SPBPQueue* q = (SPBPQueue*) malloc(sizeof(SPBPQueue));
	if (q == NULL) {
		return NULL;
	}
	(*q).size = 0;
	(*q).maxSize = maxSize;
	(*q).elements = (BPQueueElement**) calloc(maxSize, sizeof(BPQueueElement*));
	if ((*q).elements == NULL) {
		return NULL;
	}
	return q;
}

SPBPQueue* spBPQueueCopy(SPBPQueue* source) {
	SPBPQueue* q = (SPBPQueue*) malloc(sizeof(SPBPQueue));
	if (q == NULL) {
		return NULL;
	}
	(*q).size = (*source).size;
	(*q).maxSize = (*source).maxSize;
	(*q).elements = (BPQueueElement**) malloc(
			sizeof(BPQueueElement*) * ((*q).maxSize));
	for (int i = 0; i < (*source).size; i++) {
		(*q).elements[i] = (BPQueueElement*) malloc(sizeof(BPQueueElement));
		(*q).elements[i]->index = (*source).elements[i]->index;
		(*q).elements[i]->value = (*source).elements[i]->value;
	}
	return q;
}

void spBPQueueDestroy(SPBPQueue* source) {
	if (source != NULL) {
		for (int i = 0; i < source->size; i++) {
			free((source->elements[i]));
		}
		free((*source).elements);
	}
	free(source);
}

void spBPQueueClear(SPBPQueue* source) {
	(*source).size = 0;
	for (int i = 0; i < source->size; i++) {
		free((source->elements[i]));
	}
}

int spBPQueueSize(SPBPQueue* source) {
	return (*source).size;
}

int spBPQueueGetMaxSize(SPBPQueue* source) {
	return (*source).maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value) {
	// if queue is already full
	if ((*source).size == (*source).maxSize) {
		// if we can't add the index and the value
		if (value > (*source).elements[0]->value) {
			return SP_BPQUEUE_FULL;
		}
		if (value == (*source).elements[0]->value
				&& index >= (*source).elements[0]->index) {
			return SP_BPQUEUE_FULL;
		}
		// move remove biggest value element and
		BPQueueElement* toFree = (*source).elements[0];
		for (int i = 0; i < (*source).size - 1; i++) {
			(*source).elements[i] = (*source).elements[i + 1];
		}
		free(toFree);
		(*source).size--;
	}

	BPQueueElement* element = (BPQueueElement*) malloc(sizeof(BPQueueElement));
	if (element == NULL) {
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}

	(*element).value = value;
	(*element).index = index;

	int j = -1; // where we need to insert the new element

	for (int i = 0; i < (*source).size; i++) {
		if ((*source).elements[i]->value < value) {
			j = i;
			break;
		}
		if ((*source).elements[i]->value == value
				&& (*source).elements[i]->index < index) {
			j = i;
			break;
		}
	}

	if (j == -1) { // insert in the end
		(*source).elements[(*source).size] = element;
		(*source).size++;

		return SP_BPQUEUE_SUCCESS;
	}

	for (int i = (*source).size - 1; i >= j; i--) {
		(*source).elements[i + 1] = (*source).elements[i];
	}
	(*source).elements[j] = element;
	(*source).size++;
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source) {
	if ((*source).size == 0) {
		return SP_BPQUEUE_EMPTY;
	}
	free((*source).elements[(*source).size - 1]);
	(*source).size--;
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res) {
	if ((*source).size == 0) {
		return SP_BPQUEUE_EMPTY;
	}

	if (res == NULL) {
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	res->value = (source->elements)[(source->size) - 1]->value;
	res->index = (source->elements)[(source->size) - 1]->index;
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res) {
	if ((*source).size == 0) {
		return SP_BPQUEUE_EMPTY;
	}

	if (res == NULL) {
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	res->value = (source->elements)[0]->value;
	res->index = (source->elements)[0]->index;
	return SP_BPQUEUE_SUCCESS;
}

double spBPQueueMinValue(SPBPQueue* source) {
	if ((*source).size == 0) {
		return SP_BPQUEUE_EMPTY;
	}
	return (*source).elements[(*source).size - 1]->value;
}

double spBPQueueMaxValue(SPBPQueue* source) {
	if ((*source).size == 0) {
		return SP_BPQUEUE_EMPTY;
	}
	return (*source).elements[0]->value;
}

bool spBPQueueIsEmpty(SPBPQueue* source) {
	return (*source).size == 0;
}

bool spBPQueueIsFull(SPBPQueue* source) {
	return (*source).size == (*source).maxSize;
}
