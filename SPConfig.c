/*
 * SPConfig.c
 *
 *  Created on: Apr 14, 2017
 *      Author: Roey
 */

#include "SPConfig.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_STRING_SIZE 1024;

#define spPCADimension_DEFAULT 20
#define spPCAFilename_DEFAULT "pca.yml"
#define spNumOfFeatures_DEFAULT 100
#define spExtractionMode_DEFAULT true
#define spMinimalGUI_DEFAULT false
#define spNumOfSimilarImages_DEFAULT 1
#define spKNN_DEFAULT 1
#define spKDTreeSplitMethod_DEFAULT MAX_SPREAD
#define spLoggerLevel_DEFAULT 3
#define spLoggerFilename_DEFAULT "stdout"

struct sp_config_t {
	// no default values
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix;
	int spNumOfImages;
	// default values
	int spPCADimension;
	char* spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_CONFIG_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char* spLoggerFilename;
};

//TODO: use this somehow
/*
 * int spPCADimension = spPCADimension_DEFAULT;
 char* spPCAFilename = spPCAFilename_DEFAULT;
 int spNumOfFeatures = spNumOfFeatures_DEFAULT;
 bool spExtractionMode = spExtractionMode_DEFAULT;
 int spNumOfSimilarImages = spNumOfSimilarImages_DEFAULT;
 SP_CONFIG_SPLIT_METHOD spKDTreeSplitMethod = spKDTreeSplitMethod_DEFAULT;
 int spKNN = spKNN_DEFAULT;
 bool spMinimalGUI = spMinimalGUI_DEFAULT;
 int spLoggerLevel = spLoggerLevel_DEFAULT;
 char* spLoggerFilename = spLoggerFilename_DEFAULT;
 */

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!filename) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	return SP_CONFIG_SUCCESS;
	// TODO: complete
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index) {
	if (!imagePath || !config) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if (index >= config->spNumOfImages) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
			config->spImagesPrefix, index, config->spImagesSuffix);
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
}

void spConfigDestroy(SPConfig config) {
}

bool isValidConfigLine(const char* line) {
	int i = 0;
	for (; line[i] == ' '; i++) {
	}
	if (!line || line[i] == '#') { // if line contains only spaces and a comment
		return true;
	}
	char variable[1024], value[1024];
	for (int j = 0; line[i] != ' '; i++, j++) { // we reached the variable
		variable[j] = line[i];
	}
	for (; line[i] == ' '; i++) {
	}
	if (line[i] != '=') {
		return false;
	}
	i++;
	for (; line[i] == ' '; i++) {
	}
	for (int j = 0; line[i] != ' '; i++, j++) {
		value[j] = line[i];
	}
	for (i = 0; line[0] == ' '; i++) {
	}
	if (line[i] && line[i] != '#') {
		return false;
	}
	return true;
}
