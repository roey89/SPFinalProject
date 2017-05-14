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

#define MAX_STRING_SIZE 1024

#define ALLOC_ERROR_MSG "Allocation error"
#define INDEX_OUT_OF_RANGE_ERROR_MSG "Index out of range error"
#define IMREAD_ERROR_MSG "Error reading image"
#define EXIT_MSG "Exiting..."

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
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		return 0;
	}
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	if (!configMsg) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		free(loggerMsg);
		return 0;
	}
	if (!createLoggerFromConfig(config, loggerMsg, configMsg)) {
		return 0;
	}

	//Preprocessing
	spLoggerPrintDebug("Preprocessing\n", __FILE__, __func__, __LINE__);

	int numOfImages = 0;
	numOfImages = spConfigGetNumOfImages(config, configMsg);

	char* imagePath = (char*) calloc(MAX_STRING_SIZE, sizeof(char));
	SPPoint ***features_of_images = (SPPoint ***) calloc(numOfImages,
			sizeof(SPPoint **));
	int *features_per_image = (int *) calloc(numOfImages, sizeof(int));
	if (!imagePath || !features_of_images || !features_per_image) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		free(loggerMsg);
		free(configMsg);
		free(imagePath);
		free(features_of_images);
		free(features_per_image);
		return 0;
	}

	//todo seems ok till here

	bool inExtractionMode = spConfigIsExtractionMode(config, configMsg);
	int pointsDim = spConfigGetPCADim(config, configMsg);

	int num_of_all_features = 0;

	ImageProc* proc = new ImageProc(config);

	for (int i = 0; i < numOfImages; i++) {
		spConfigGetImagePath(imagePath, config, i);
		if (*configMsg == SP_CONFIG_INDEX_OUT_OF_RANGE) {
			spLoggerPrintError(INDEX_OUT_OF_RANGE_ERROR_MSG, __FILE__, __func__,
			__LINE__);
			spConfigDestroy(config);
			spLoggerDestroy();
			free(loggerMsg);
			free(configMsg);
			free(imagePath);
			free(features_of_images);
			free(features_per_image);
			delete (proc);
			return 0;
		}
		if (inExtractionMode) {
			features_of_images[i] = proc->getImageFeatures(imagePath, i,
					features_per_image + i);
			if (!features_of_images[i]) {
				spLoggerPrintError(IMREAD_ERROR_MSG, __FILE__, __func__,
				__LINE__);
				for (int i = 0; i < numOfImages; i++) {
					for (int j = 0; j < features_per_image[i]; j++) {
						free(features_of_images[i][j]);
					}
					free(features_of_images[i]);
				}
				free(features_of_images);
				spConfigDestroy(config);
				spLoggerDestroy();
				free(loggerMsg);
				free(configMsg);
				free(imagePath);
				free(features_of_images);
				free(features_per_image);
				delete (proc);
				return 0;
			}
			storeFeatures(imagePath, features_of_images[i], i,
					*(features_per_image + i), pointsDim);
		} else {
			features_of_images[i] = loadFeatures(imagePath,
					features_per_image + i);
		}
		num_of_all_features += features_per_image[i];
	}

	//todo
	char* queryPath = (char*) calloc(MAX_STRING_SIZE, sizeof(char));
	if (!queryPath) {
		//todo free
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		free(imagePath);
		free(features_of_images);
		return 0;
	}

	getQueryDirectoryFromUser(queryPath);
	if (strcmp(queryPath, "<>") == 0) {
		printf(EXIT_MSG);
	}

	int features_of_image = 0;
	SPPoint **image_feats = proc->getImageFeatures(queryPath, 0,
			&features_of_image);
	if (!image_feats) {
		// todo free
	}
	SPPoint **features = (SPPoint **) calloc(num_of_all_features,
			sizeof(SPPoint *));
	if (!features) {
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
	free(features_of_images);

	images_near_values = (int *) calloc(numOfImages, sizeof(int));
	if (images_near_values) {
		// todo free
	}

	KDTreeNode *tree = init_tree(features, num_of_all_features);
	if (!tree) {
		// todo free
	}
	BPQueueElement* res = (BPQueueElement *) malloc(sizeof(BPQueueElement));
	if (!res) {
		// todo free
	}
	for (int i = 0; i < features_of_image; i++) {
		SPBPQueue *queue = find_k_nearest(tree, image_feats[i], config);
		if (!queue) {
			// todo free
		}
		while (!spBPQueueIsEmpty(queue)) {
			spBPQueuePeek(queue, res);
			spBPQueueDequeue(queue);
			images_near_values[(*res).index]++;
		}
		spBPQueueDestroy(queue);
	}
	free(res);

	int *best_indexes = (int *) calloc(numOfImages, sizeof(int));
	if (!best_indexes) {
		// todo free
	}
	for (int i = 0; i < numOfImages; i++) {
		best_indexes[i] = i;
	}

	std::qsort(best_indexes, numOfImages, sizeof(int), reverse_comp);
	free(images_near_values);

	int k = spConfigGetNumOfSimilarImages(config, configMsg); //k-nearest
	int *k_nearest = (int *) calloc(k, sizeof(int));
	if (!k_nearest) {
		// todo free
	}

	for (int i = 0; i < k; i++) {
		k_nearest[i] = best_indexes[i];
	}
	free(best_indexes);

	//free image_feats
	for (int i = 0; i < features_of_image; i++) {
		spPointDestroy(image_feats[i]);
	}
	free(image_feats);

	free(loggerMsg);

	minimalGui(config, k, configMsg, imagePath, k_nearest, proc, queryPath);
	return 0;
}
