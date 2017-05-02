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
#define INVALID_VARIABLE_STRING "File: %s\nLine: %d\nMessage: Invalid variable - the variable %s does not exist in the config"
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
void setConfigDefaultValues(SPConfig config);
void freeBeforeExit(SPConfig config, FILE* fp, char* line, char* variable,
		char* value);

//Getters
char* getVariableName(const char* line);
char* getVariableValue(const char* line);

//Constraints
bool isValidBoolean(const char* string);
bool positiveNum(int num);
bool noSpacesString(char* str);
bool spImagesDirectoryConstraint(char* directory);
bool spImagesPrefixConstraint(char* prefix);
bool spImagesSuffixConstraint(char* suffix);
bool spNumOfImagesConstraint(int numOfImages);
bool spPCADimensionConstraint(int dim);
bool spPCAFilenameConstraint(char* filename);
bool spNumOfFeaturesConstraint(int numOfFeatures);
bool spExtractionModeConstraint(char* extractionMode);
bool spNumOfSimilarImagesConstraint(int numOfSimilarImages);
bool spKDTreeSplitMethodConstraint(char* method);
bool spKNNConstraint(int spKNN);
bool spMinimalGUIConstraint(char* minimalGUI);
bool spLoggerLevelConstraint(int level);
bool spLoggerFilenameConstraint(char* filename);

//Parsers
SP_CONFIG_SPLIT_METHOD spKDTreeSplitMethodParser(char* method);
bool booleanParser(char* boolean);

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

void setConfigDefaultValues(SPConfig config) {
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
}

void freeBeforeExit(SPConfig config, FILE* fp, char* line, char* variable,
		char* value) {
	fclose(fp);
	spConfigDestroy(config);
	spLoggerDestroy();
	free(line);
	free(variable);
	free(value);
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	// TODO Add macros and auxiliary functions!
	assert(msg != NULL);

	if (!filename) {
		spLoggerDestroy();
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	FILE * fp = fopen(filename, SP_CONFIG_OPEN_MODE);
	if (!fp) {
		spLoggerDestroy();
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}

	SPConfig config = (SPConfig) malloc(sizeof(SPInnerConfig));
	if (!config) {
		fclose(fp);
		spLoggerDestroy();
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	setConfigDefaultValues(config);	//setting default values

	// going over the file
	int charNum, lineNum = 0;
	bool spImagesDirectorySet = false, spImagesPrefixSet = false,
			spImagesSuffixSet = false, spNumOfImagesSet = false;
	char currChar, *line = (char*) calloc(MAX_STRING_SIZE, sizeof(char));
	if (!line) {
		fclose(fp);
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	while (!feof(fp)) {
		line = "";
		charNum = 0;
		lineNum++;
		while (!feof(fp) && (currChar = fgetc(fp)) != '\n') {
			line[charNum++] = currChar;
		}
		if (!isValidConfigLine(line)) {
			fclose(fp);
			free(line);
			spConfigDestroy(config);
			spLoggerDestroy();
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return NULL;
		}

		char *variable, *value;
		variable = getVariableName(line);

		if (!variable) { //If line is a comment or empty
			continue;
		}

		if (strcmp(variable, " ") == 0) {
			free(line);
			fclose(fp);
			spConfigDestroy(config);
			spLoggerDestroy();
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}

		value = getVariableValue(line);
		if (strcmp(value, " ") == 0) {
			free(line);
			free(variable);
			fclose(fp);
			spConfigDestroy(config);
			spLoggerDestroy();
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}

		if (strcmp(variable, "spImagesDirectory") == 0) {
			if (spImagesDirectoryConstraint(value)) {
				config->spImagesDirectory = value;
				spImagesDirectorySet = true;
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_STRING;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spImagesPrefix") == 0) {
			if (spImagesPrefixConstraint(value)) {
				config->spImagesPrefix = value;
				spImagesPrefixSet = true;
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_STRING;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spImagesSuffix") == 0) {
			if (spImagesSuffixConstraint(value)) {
				config->spImagesSuffix = value;
				spImagesSuffixSet = true;
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_STRING;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spNumOfImages") == 0) {
			if (spNumOfImagesConstraint(atoi(value))) {
				config->spNumOfImages = atoi(value);
				free(value);
				spNumOfImagesSet = true;
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_INTEGER;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spPCADimension") == 0) {
			if (spPCADimensionConstraint(atoi(value))) {
				config->spPCADimension = atoi(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_INTEGER;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}

		} else if (strcmp(variable, "spPCAFilename") == 0) {
			if (spPCAFilenameConstraint(value)) {
				config->spPCAFilename = value;
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_STRING;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spNumOfFeatures") == 0) {
			if (spNumOfFeaturesConstraint(atoi(value))) {
				config->spNumOfFeatures = atoi(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_INTEGER;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}

		} else if (strcmp(variable, "spExtractionMode") == 0) {
			if (spExtractionModeConstraint(value)) {
				config->spExtractionMode = booleanParser(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_BOOLEAN;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}

		} else if (strcmp(variable, "spNumOfSimilarImages") == 0) {
			if (spNumOfSimilarImagesConstraint(atoi(value))) {
				config->spNumOfSimilarImages = atoi(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_INTEGER;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}

		} else if (strcmp(variable, "spKDTreeSplitMethod") == 0) {
			if (spKDTreeSplitMethodConstraint(value)) {
				config->spMinimalGUI = spKDTreeSplitMethodParser(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_SPLIT_METHOD;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spKNN") == 0) {
			if (spKNNConstraint(atoi(value))) {
				config->spKNN = atoi(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_INTEGER;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spMinimalGUI") == 0) {
			if (spMinimalGUIConstraint(value)) {
				config->spMinimalGUI = booleanParser(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_BOOLEAN;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spLoggerLevel") == 0) {
			if (spLoggerLevelConstraint(atoi(value))) {
				config->spLoggerLevel = atoi(value);
				free(value);
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_INTEGER;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else if (strcmp(variable, "spLoggerFilename") == 0) {
			if (spPCAFilenameConstraint(value)) {
				config->spLoggerFilename = value;
			} else {
				freeBeforeExit(config, fp, line, variable, value);
				*msg = SP_CONFIG_INVALID_STRING;
				printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);
				return NULL;
			}
		} else { // variable is invalid
			printf(INVALID_VARIABLE_STRING, filename, lineNum, variable);
			freeBeforeExit(config, fp, line, variable, value);
			*msg = SP_CONFIG_INVALID_VARIABLE_NAME;
			return NULL;
		}
		free(variable);
	}

	fclose(fp);
	free(line);

	//Check if variables without default values has been set
	if (!spImagesDirectorySet) {
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_DIR;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum,
				"spImagesDirectory");
		return NULL;
	}
	if (!spImagesPrefixSet) {
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_PREFIX;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum, "spImagesPrefix");
		return NULL;

	}
	if (!spImagesSuffixSet) {
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_SUFFIX;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum, "spImagesSuffix");
		return NULL;
	}
	if (!spNumOfImagesSet) {
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum, "spNumOfImages");
		return NULL;
	}

	*msg = SP_CONFIG_SUCCESS;
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
	if (!config) {
		return;
	}
	free(config->spImagesDirectory);
	free(config->spImagesPrefix);
	free(config->spImagesSuffix);
	free(config->spPCAFilename);
	free(config->spLoggerFilename);
	free(config);
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

bool isValidBoolean(const char* string) {
	return strcmp(string, "false") == 0 || strcmp(string, "true") == 0;
}

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

bool spExtractionModeConstraint(char* extractionMode) {
	return isValidBoolean(extractionMode);
}

bool spNumOfSimilarImagesConstraint(int numOfSimilarImages) {
	return positiveNum(numOfSimilarImages);
}

bool spKDTreeSplitMethodConstraint(char* method) {
	if (strcmp(method, "RANDOM") == 0 || strcmp(method, "MAX_SPREAD") == 0
			|| strcmp(method, "INCREMENTAL") == 0) {
		return true;
	}
	return false;
}

bool spKNNConstraint(int spKNN) {
	return positiveNum(spKNN);
}

bool spMinimalGUIConstraint(char* minimalGUI) {
	return isValidBoolean(minimalGUI);
}

bool spLoggerLevelConstraint(int level) {
	return level >= 1 && level <= 4;
}

bool spLoggerFilenameConstraint(char* filename) {
	return noSpacesString(filename);
}

/////////////
// Parsers //
/////////////

SP_CONFIG_SPLIT_METHOD spKDTreeSplitMethodParser(char* method) {
// we assume method is valid
	if (strcmp(method, "RANDOM") == 0) {
		return RANDOM;
	} else if (strcmp(method, "MAX_SPREAD") == 0) {
		return MAX_SPREAD;
	} else { //if (strcmp(method, "INCREMENTAL") == 0)
		return INCREMENTAL;
	}
}

bool booleanParser(char* boolean) {
// we assume boolean is valid
	return strcmp(boolean, "true") == 0;
}
