/*
 * main_aux.cpp
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>

//C source files
extern "C" {
#include "SPConfig.h"
#include "SPPoint.h"
}
#include "main_aux.h"

#define CANT_OPEN_DEFAULT_CONFIG_FILE_ERROR_MSG "The default configuration file spcbir.config couldn't be open\n"
#define CANT_OPEN_CONFIG_FILE_ERROR_MSG "The configuration file %s couldn't be open\n"
#define INVALID_CONFIF_COMMAND_LINE_ERROR "Invalid command line : use -c <config_filename>\n"
#define ALLOC_ERROR_MSG "Allocation error"

char* getFeatsDir(char* imagePath);

SPConfig createConfigFromCmd(int argc, char** argv) {
	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	if (!msg) {
		return NULL;
	}
	SPConfig config;
	if (argc == 1) {
		config = spConfigCreate("spcbir.config", msg);
		if (*msg == SP_CONFIG_SUCCESS) {
			free(msg);
			return config;
		}
		if (*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(
			CANT_OPEN_DEFAULT_CONFIG_FILE_ERROR_MSG);
		}
		free(msg);
		return NULL;
	} else if (argc == 3) {
		if (strcmp(argv[1], "-c") == 0) {
			config = spConfigCreate(argv[2], msg);
			if (*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
				printf(CANT_OPEN_CONFIG_FILE_ERROR_MSG, argv[2]);
			}
			free(msg);
			return config;
		}
	}
	printf(INVALID_CONFIF_COMMAND_LINE_ERROR);
	free(msg);
	return NULL;
}

SP_LOGGER_LEVEL intToLoggerLevel(int num) {
	assert(num < 5 && num > 0);
	if (num == 1) {
		return SP_LOGGER_ERROR_LEVEL;
	} else if (num == 2) {
		return SP_LOGGER_WARNING_ERROR_LEVEL;
	} else if (num == 3) {
		return SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
	} else { // num == 4
		return SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
	}
}

bool createLoggerFromConfig(SPConfig config, SP_LOGGER_MSG* loggerMsg,
		SP_CONFIG_MSG* configMsg) {
	char* loggerFileName = spConfigGetLoggerFilename(config, configMsg);
	if (*configMsg == SP_CONFIG_ALLOC_FAIL) {
		printf(ALLOC_ERROR_MSG);
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		return false;
	}
	int loggerLevel = spConfigGetLoggerLevel(config, configMsg);
	*loggerMsg = spLoggerCreate(loggerFileName, intToLoggerLevel(loggerLevel)); //todo print errors
	free(loggerFileName);
	if (*configMsg == SP_CONFIG_SUCCESS) {
		return true;
	} else { // todo print error depending on msg
		spConfigDestroy(config);
		free(loggerMsg);
		free(configMsg);
		return false;
	}
}

void getQueryDirectoryFromUser(char* str) {
	printf("Please enter an image path:\n");
	scanf("%s", str);
}

// dim - feature size
void storeFeatures(char* imageDir, SPPoint ** features, int imageIndex,
		int numberOfFeatures, int pointDim) {
	char* featsDir = getFeatsDir(imageDir);
	if (!featsDir) {
		//todo
	}
	FILE* fp = fopen(featsDir, "w"); //todo
	if (!fp) {
		//todo
	}
	free(featsDir);
	fprintf(fp, "%d\n%d\n%d\n", imageIndex, numberOfFeatures, pointDim);
	for (int i = 0; i < numberOfFeatures; i++) {
		for (int j = 0; j < pointDim; j++) {
			fprintf(fp, "%lf\n", spPointGetAxisCoor(features[i], j));
		}
	}
	fclose(fp);
}
SPPoint ** loadFeatures(char* imageDir, int* numberOfFeatures) {
	char* featsDir = getFeatsDir(imageDir);
	if (!featsDir) {
		//todo
	}
	FILE* fp = fopen(featsDir, "r"); //todo
	if (!fp) {
		//todo
	}
	free(featsDir);
	int imageIndex, pointDim;
	fscanf(fp, "%d\n%d\n%d\n", &imageIndex, numberOfFeatures, &pointDim);
	SPPoint ** features = (SPPoint**) calloc(*numberOfFeatures,
			sizeof(SPPoint*)); //todo
	double* data = (double*) calloc(pointDim, sizeof(double)); // todo
	for (int i = 0; i < *numberOfFeatures; i++) {
		memset(data, 0, pointDim * sizeof(double));
		for (int j = 0; j < pointDim; j++) {
			fscanf(fp, "%lf", &data[j]);
		}
		features[i] = spPointCreate(data, pointDim, imageIndex); //todo
	}
	free(data);
	fclose(fp);
	return features;
}

// imagePath should be in the set { .jpg , .png , .bmp , .gif }
char* getFeatsDir(char* imagePath) {
	char* dir = (char*) calloc(strlen(imagePath) + 3, sizeof(char));
	if (!dir) {
		//todo
	}
	memcpy(dir, imagePath, strlen(imagePath) - 3);
	memcpy(dir + strlen(dir), "feats", 5);
	return dir;
}

bool minimalGui(SPConfig config, int k, SP_CONFIG_MSG* configMsg,
		char* imagePath, int* k_nearest, ImageProc* proc, char* queryPath) {
	if (!config) {
		spLoggerPrintError("Invadlid argument: config is NULL", __FILE__,
				__func__,
				__LINE__);
		free(configMsg);
		free(imagePath);
		free(k_nearest);
		free(queryPath);
		delete (proc);
		return false;
	}
	bool minimalGui = spConfigMinimalGui(config, configMsg);
	if (!minimalGui) {
		printf("Best candidates for - %s - are:\n", queryPath);
	}
	for (int i = 0; i < k; i++) {
		spConfigGetImagePath(imagePath, config, k_nearest[i]);
		if (*configMsg == SP_CONFIG_INDEX_OUT_OF_RANGE) {
			spLoggerPrintError("Index out of range", __FILE__, __func__,
			__LINE__);
			spConfigDestroy(config);
			free(configMsg);
			free(imagePath);
			free(k_nearest);
			free(queryPath);
			delete (proc);
			return false;
		}
		if (minimalGui) {
			proc->showImage(imagePath);
		} else {
			printf("%s\n", imagePath);
		}
	}
	spConfigDestroy(config);
	free(configMsg);
	free(imagePath);
	free(k_nearest);
	free(queryPath);
	delete (proc);
	return true;
}

