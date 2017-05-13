/*
 * main.cpp
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#include <cstdlib>

//C source files
extern "C" {
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPKDNode.h"
#include "SPBPriorityQueue.h"
}

//C++ source files
#include "SPImageProc.h"
#include "main_aux.h"

using namespace sp;

//todo whereever there is a get_from_config function, check msg!=succ bla bla
//todo print config null error, shouldnt happen...

static int *images_near_values;

int reverse_comp(const void *x, const void *y) {
	int x_val = images_near_values[*((int *) x)];
	int y_val = images_near_values[*((int *) y)];
	if (x_val > y_val) {
		return -1;
	}
	if (x_val < y_val) {
		return 1;
	}
	return 0;
}

int main(int argc, char** argv) {
	SPConfig config = createConfigFromCmd(argc, argv);
	if (!config) {
		return 0;
	}
	SP_LOGGER_MSG* loggerMsg = (SP_LOGGER_MSG*) malloc(sizeof(SP_LOGGER_MSG));
	if (!loggerMsg) {
		spLoggerPrintError("Allocation error!", __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		return 0;
	}
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	if (!configMsg) {
		spLoggerPrintError("Allocation error!", __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		free(loggerMsg);
		return 0;
	}
	if (!createLoggerFromConfig(config, loggerMsg, configMsg)) {
		return 0;
	}

// Preprocessing todo save and load to/from file

	int numOfImages = 0;
	numOfImages = spConfigGetNumOfImages(config, configMsg);
	if (*configMsg == SP_CONFIG_INVALID_ARGUMENT) {
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		return 0;
	}

	ImageProc* proc = new ImageProc(config);

	//todo
	char* imagePath = (char*) calloc(128, sizeof(char));
	if (!imagePath) {
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		return 0;
	}
	SPPoint ***features_of_images = (SPPoint ***) calloc(numOfImages,
			sizeof(SPPoint **));
	if (!features_of_images) {
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		free(imagePath);
		return 0;
	}
	int *features_per_image = (int *) calloc(numOfImages, sizeof(int));
	if (!features_per_image) {
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		free(imagePath);
		free(features_of_images);
		return 0;
	}
	int num_of_all_features = 0;
	bool inExtractionMode = spConfigIsExtractionMode(config, configMsg);
	if (*configMsg == SP_CONFIG_INVALID_ARGUMENT) {
		//todo
	}
	int pointsDim = spConfigGetNumOfFeatures(config, configMsg);
	if (*configMsg == SP_CONFIG_INVALID_ARGUMENT) {
		//todo
	}
	for (int i = 0; i < numOfImages; i++) {
		spConfigGetImagePath(imagePath, config, i);
		if (inExtractionMode) {
			features_of_images[i] = proc->getImageFeatures(imagePath, i,
					features_per_image + i); // +i?
			storeFeatures(imagePath, features_of_images[i], i,
					features_per_image[i], pointsDim);
		} else {
			features_of_images[i] = loadFeatures(imagePath, &features_per_image[i]);
		}
		num_of_all_features += features_per_image[i];
	}

	//todo
	char* queryPath = (char*) calloc(128, sizeof(char));
	if (!queryPath) {
		//todo free
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		free(imagePath);
		free(features_of_images);
		return 0;
	}
//	queryPath = getQueryDirectoryFromUser(queryPath); todo uncomment this and delete next line
	strcpy(queryPath, "./Images/imag100.png");

	int features_of_image = 0;
	SPPoint **image_feats = proc->getImageFeatures(queryPath, 0,
			&features_of_image);

	SPPoint **features = (SPPoint **) calloc(num_of_all_features,
			sizeof(SPPoint *));
	if (features == NULL) {
		// todo free
	}
	int features_index = 0;
	for (int i = 0; i < numOfImages; i++) {
		for (int j = 0; j < features_per_image[i]; j++) {
			features[features_index++] = spPointCopy(features_of_images[i][j]);
			free(features_of_images[i][j]);
		}
		free(features_of_images[i]);
	}
	images_near_values = (int *) calloc(numOfImages, sizeof(int));
	if (images_near_values) {
		// todo free
	}

	KDTreeNode *tree = init_tree(features, num_of_all_features);
	BPQueueElement* res = (BPQueueElement *) malloc(sizeof(BPQueueElement));
	for (int i = 0; i < features_of_image; i++) {
		SPBPQueue *queue = find_k_nearest(tree, image_feats[i], config);
		while (!spBPQueueIsEmpty(queue)) {
			spBPQueuePeek(queue, res);
			spBPQueueDequeue(queue);
			images_near_values[(*res).index]++;

		}
		spBPQueueDestroy(queue);
	}
	free(res);

	int *best_indexes = (int *) calloc(numOfImages, sizeof(int));
	for (int i = 0; i < numOfImages; i++) {
		best_indexes[i] = i;
	}

	std::qsort(best_indexes, numOfImages, sizeof(int), reverse_comp);

	int k = spConfigGetNumOfSimilarImages(config, configMsg); //nearest k, todo msg!=bla bla
	int *k_nearest = (int *) calloc(k, sizeof(int));

	for (int i = 0; i < k; i++) {
		k_nearest[i] = best_indexes[i];
	}

	free(best_indexes);
	free(images_near_values);
	//free image_feats
	for (int i = 0; i < features_of_image; i++) {
		spPointDestroy(image_feats[i]);
	}

	free(loggerMsg);
	free(image_feats);

	minimalGui(config, k, configMsg, imagePath, k_nearest, proc, queryPath);
	return 0;
}
