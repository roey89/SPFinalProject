#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 */

/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * creates a new priority queue that can contain 'maxSize' elements
 * @param maxSize - the maximum number of values the queue could contain
 * @return a pointer to that queue
 */
SPBPQueue* spBPQueueCreate(int maxSize);

/**
 * creates a new queue and copies the source queue into it (that is an exact copy of the source queue
 * @param source - the source queue
 * @return a queue that has the exact same values that are stored in the source queue
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * frees all storage allocated for the queue that source points at.
 * @param source - the source queue
 */
void spBPQueueDestroy(SPBPQueue* source);

/**
 * resets the data saved in the queue, after a call to this function source will store no data,
 * but be an empty queue that can contain the same number of elements as before
 * @param source - the source queue
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * returns the number of elemnets stored in the queue
 * @param source - the source queue
 * @return the number of value stored in source
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * returns the maximal number of elements that can e stored in the queue
 * @param source - the source queue
 * @return the maximal number of values that source can contain
 */
int spBPQueueGetMaxSize(SPBPQueue* source);

/**
 * insert a new element to the queue.
 * if the queue is full:
 * 1.then if the value recieved as parameter is bigger than every other value that stored in the queue it wont be inserted into the queue
 * 2. if the value recieved as parameter equals to the bigger value stored in the queue then: if the index that was recieved as a parameter is
 * bigger then the index of the highest value in the queue, then the new element wont be inserted into the queue
 * * 3. otherwise, the element with the highest value, and that has the highest index between those elements will be removed from the queue, and a new element
 * that contains the value and index that were recieved as parameters will be inserted nto the queue
 *
 * the function will return a message
 * @param source - the source queue
 * @param index - the index of the new element that will be added to the queue
 *  @param value - the value of the new element that will be added to the queue
 *  @return a message that gives information about the success of the enqueue
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * removes the element with the lowest value from the queue
 * @param source - the source queue
 * @return a message that gives information about the success of the enqueue
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * Returns a copy of the element with the lowest value(or one of the lowest values elements, if there are many), res (recieved as parameter) will be pointing to that element
 * if there are no elements inside the queue then res will point to null and a correct message will be send as a result
 * @param source - the source queue
 * @param ret -  a pointer to a queue element that will point at the lowest value element at the end of the run of the function
 * @return a message that gives information about the success of the function
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * Returns a copy of the element with the highest value(or one of the highest values elements, if there are many) , res (recieved as parameter) will be pointing to that element
 * @param source - the source queue
 * @param ret -  a pointer to a queue element that will point at the highest value element at the end of the run of the function
 * @return a message that gives information about the success of the function
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * returns the minimum value that is stored in the queue the source points at
 * @param source - the source queue
 * @return the minimal value that is stored in the source queue
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * returns the maximum value that is stored in the queue the source points at
 * @param source - the source queue
 * @return the maximal value that is stored in the source queue
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * returns true if the queue the source point at is empty - wich means - if the queue stores 0 values
 * @param source - the source queue
 * @return true - if there are no values stored in the source queue, false - otherwise
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * returns true if the queue the source point at is full - wich means - if the queue stores 'MaxSize' (the maximum size of of elements it can contain) values
 * @param source - the source queue
 * @return true - if the source queue cant contain any more values - which means its full, false- otherwise
 */
bool spBPQueueIsFull(SPBPQueue* source);

#endif
