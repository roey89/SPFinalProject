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
#include <ctype.h>
#include <string.h>

#define MAX_STRING_SIZE 1024

//File open mode
#define SP_CONFIG_OPEN_MODE "r"

//Error messages
#define INVALID_LINE_STRING "File: %s\nLine: %d\nMessage: Invalid configuration line"
#define CONSTRAINT_NOT_MET_STRING "File: %s\nLine: %d\nMessage: Invalid value - constraint not met"
#define PARAMETER_NOT_SET_STRING "File: %s\nLine: %d\nMessage: Parameter %s is not set"

//Default values
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

bool isValidConfigLine(const char* line);
char* getVariableName(const char* line);
char* getVariableValue(const char* line);

// Constraints
bool positiveNum(int num);
bool noSpacesString(char* str);
bool spImagesDirectoryConstraint(char* directory);
bool spImagesPrefixConstraint(char* prefix);
bool spImagesSuffixConstraint(char* suffix);
bool spNumOfImagesConstraint(int numOfImages);
bool spPCADimensionConstraint(int dim);
bool spPCAFilenameConstraint(char* filename);
bool spNumOfFeaturesConstraint(int numOfFeatures);
bool spExtractionModeConstraint(bool extractionMode);
bool spNumOfSimilarImagesConstraint(int numOfSimilarImages);
bool spKDTreeSplitMethodConstraint(SP_CONFIG_SPLIT_METHOD method);
bool spKNNConstraint(int spKNN);
bool spMinimalGUIConstraint(bool minimalGUI);
bool spLoggerLevelConstraint(int level);
bool spLoggerFilenameConstraint(char* filename);

typedef struct sp_config_t {
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
} SPInnerConfig;

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);

	if (!filename) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	FILE * fp = fopen(filename, SP_CONFIG_OPEN_MODE);
	if (!fp) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}

	SPConfig config = (SPConfig) malloc(sizeof(SPInnerConfig));
	if (!config) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	// setting default values
	config->spPCADimension = spPCADimension_DEFAULT;
	config->spPCAFilename = spPCAFilename_DEFAULT;
	config->spNumOfFeatures = spNumOfFeatures_DEFAULT;
	config->spExtractionMode = spExtractionMode_DEFAULT;
	config->spNumOfSimilarImages = spNumOfSimilarImages_DEFAULT;
	config->spKDTreeSplitMethod = spKDTreeSplitMethod_DEFAULT;
	config->spKNN = spKNN_DEFAULT;
	config->spMinimalGUI = spMinimalGUI_DEFAULT;
	config->spLoggerLevel = spLoggerLevel_DEFAULT;
	config->spLoggerFilename = spLoggerFilename_DEFAULT;

	// going over the file
	char currChar, *line = (char*) calloc(MAX_STRING_SIZE, sizeof(char));
	int charNum, lineNum = 0;
	bool spImagesDirectorySet = false, spImagesPrefixSet = false,
			spImagesSuffixSet = false, spNumOfImagesSet = false;

	while (!feof(fp)) {
		line = "";
		charNum = 0;
		lineNum++;
		while (!feof(fp) && (currChar = fgetc(fp)) != '\n') {
			line[charNum++] = currChar;
		}
		if (!isValidConfigLine(line)) {
			spConfigDestroy(config);
			spLoggerDestroy();
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return NULL;
		}

		char* variable = getVariableName(line);
		if (variable) { //If line is not a comment or empty
			if (strcmp(variable, " ") == 0) {
				spConfigDestroy(config);
				spLoggerDestroy();
				*msg = SP_CONFIG_ALLOC_FAIL;
				return NULL;
			}
			char* value = getVariableValue(line);
			if (strcmp(value, " ") == 0) {
				free(variable);
				spConfigDestroy(config);
				spLoggerDestroy();
				*msg = SP_CONFIG_ALLOC_FAIL;
				return NULL;
			}
		}
	}

	//Check if variables without default values has been set
	if (!spImagesDirectorySet) {
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_DIR;
		printf("File: %s\nLine: %d\nMessage: Parameter %s is not set", filename,
				lineNum, "spImagesDirectory");
		return NULL;
	}
	if (!spImagesPrefixSet) {
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_PREFIX;
		printf("File: %s\nLine: %d\nMessage: Parameter %s is not set", filename,
				lineNum, "spImagesPrefix");

	}
	if (!spImagesSuffixSet) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_SUFFIX;
		printf("File: %s\nLine: %d\nMessage: Parameter %s is not set", filename,
				lineNum, "spImagesSuffix");

	}
	if (!spNumOfImagesSet) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		printf("File: %s\nLine: %d\nMessage: Parameter %s is not set", filename,
				lineNum, "spNumOfImages");
	}

	fread(line, sizeof(char), MAX_STRING_SIZE, fp);
	*msg = SP_CONFIG_SUCCESS;
	// TODO complete
	return config;
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
// We assume imagePath has enough space in it
	sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
			config->spImagesPrefix, index, config->spImagesSuffix);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	if (!pcaPath || !config) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
// We assume pcaPath has enough space in it
	sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPCAFilename);
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config) {
}

bool isValidConfigLine(const char* line) {
	int i = 0;
	for (; isspace(line[i]); i++) {
	}
	if (line[i] == '\0' || line[i] == '\n' || line[i] == '#') { // if line contains only spaces and a comment
		return true;
	}
	char variable[MAX_STRING_SIZE], value[MAX_STRING_SIZE];
	for (int j = 0; !isspace(line[i]); i++, j++) { // we reached the variable
		variable[j] = line[i];
	}
	if (strlen(variable) == 0) {
		return false;
	}
	for (; isspace(line[i]); i++) {
	}
	if (line[i] != '=') {
		return false;
	}
	i++;
	for (; isspace(line[i]); i++) {
	}
	for (int j = 0; !isspace(line[i]); i++, j++) {
		value[j] = line[i];
	}
	if (strlen(value) == 0) {
		return false;
	}
	for (i = 0; isspace(line[i]); i++) {
	}
	return (line[i] == '\0' || line[i] == '\n' || line[i] == '#');
}

char* getVariableName(const char* line) {
	// We assume line is valid
	int i = 0;
	for (; isspace(line[i]); i++) {
	}
	if (line[i] == '\0' || line[i] == '\n' || line[i] == '#') { // if line contains only spaces and a comment
		return NULL;
	}
	char* variable = (char*) calloc(MAX_STRING_SIZE, sizeof(char));
	if (!variable) {
		return "";
	}
	for (int j = 0; !isspace(line[i]); i++, j++) { // we reached the variable
		variable[j] = line[i];
	}
	return variable;
}

char* getVariableValue(const char* line) {
	int i = 0;
	for (; isspace(line[i]); i++) {
	}
	if (line[i] == '\0' || line[i] == '\n' || line[i] == '#') { // if line contains only spaces and a comment
		return NULL;
	}
	char* value = (char*) calloc(MAX_STRING_SIZE, sizeof(char));
	if (!value) {
		return "";
	}
	for (int j = 0; !isspace(line[i]); i++, j++) { // we reached the variable
	}
	for (; line[i] != '='; i++) {
	}
	for (; isspace(line[i]); i++) {
	}
	for (int j = 0; !isspace(line[i]); i++, j++) {
		value[j] = line[i];
	}
	if (strlen(value) == 0) {
		return NULL;
	}
	return value;
}

/////////////////
// Constraints //
/////////////////

bool noSpacesString(char* str) {
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == ' ') {
			return false;
		}
	}
	return true;
}

bool positiveNum(int num) {
	return num > 0;
}

bool spImagesDirectoryConstraint(char* directory) {
	return noSpacesString(directory);
}

bool spImagesPrefixConstraint(char* prefix) {
	return noSpacesString(prefix);
}

bool spImagesSuffixConstraint(char* suffix) {
	char* arr[4] = { ".jpg", ".png", ".bmp", ".gif" };
	for (int i = 0; i < 4; i++) {
		if (strcmp(suffix, arr[i]) == 0) {
			return true;
		}
	}
	return false;
}

bool spNumOfImagesConstraint(int numOfImages) {
	return positiveNum(numOfImages);
}

bool spPCADimensionConstraint(int dim) {
	return dim >= 10 && dim <= 28;
}

bool spPCAFilenameConstraint(char* filename) {
	return noSpacesString(filename);
}

bool spNumOfFeaturesConstraint(int numOfFeatures) {
	return positiveNum(numOfFeatures);
}

bool spExtractionModeConstraint(bool extractionMode) { // just for clarification
	return true;
}

bool spNumOfSimilarImagesConstraint(int numOfSimilarImages) {
	return positiveNum(numOfSimilarImages);
}

bool spKDTreeSplitMethodConstraint(SP_CONFIG_SPLIT_METHOD method) { // just for clarification
	return true;
}

bool spKNNConstraint(int spKNN) {
	return positiveNum(spKNN);
}

bool spMinimalGUIConstraint(bool minimalGUI) { // just for clarification
	return true;
}

bool spLoggerLevelConstraint(int level) {
	return level >= 1 && level <= 4;
}

bool spLoggerFilenameConstraint(char* filename) {
	return noSpacesString(filename);
}
