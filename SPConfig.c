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

#define MAX_STRING_SIZE 1024

//File open mode
#define SP_CONFIG_OPEN_MODE "r"

//Error messages
#define INVALID_LINE_STRING "File: %s\nLine: %d\nMessage: Invalid configuration line\n"
#define CONSTRAINT_NOT_MET_STRING "File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n"
#define PARAMETER_NOT_SET_STRING "File: %s\nLine: %d\nMessage: Parameter %s is not set\n"

#define MAP_CASE_STRING_CONSTRAINT_NOT_MET \
	*msg = SP_CONFIG_INVALID_STRING; \
	printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum); \
	return false;

#define MAP_CASE_INTEGER_CONSTRAINT_NOT_MET \
	*msg = SP_CONFIG_INVALID_INTEGER; \
	printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum); \
	return false;

#define MAP_CASE_BOOLEAN_CONSTRAINT_NOT_MET \
	*msg = SP_CONFIG_INVALID_BOOLEAN;\
	printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);\
	return false;

#define MAP_CASE_SPLIT_METHOD_CONSTRAINT_NOT_MET \
	*msg = SP_CONFIG_INVALID_SPLIT_METHOD;\
	printf(CONSTRAINT_NOT_MET_STRING, filename, lineNum);\
	return false;

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

//General auxiliary functions
void setConfigInitialValues(SPConfig config);
bool mapVarAndValToConfig(SPConfig config, char* variable, char* value,
		const char* filename, int lineNum, SP_CONFIG_MSG* msg);
bool allVariablesSet(SPConfig config, const char* filename, int lineNum,
		SP_CONFIG_MSG* msg);
void freeBeforeExit(SPConfig config, FILE* file, char* line, char* variable,
		char* value);
bool isValidBoolean(const char* string);
bool positiveNum(int num);
bool isInt(char* numString);
bool noSpacesString(char* str);

//Line parsers
bool isValidConfigLine(char* line);
char* getVariableName(const char* line);
char* getVariableValue(const char* line);

//String parsers
SP_CONFIG_SPLIT_METHOD spKDTreeSplitMethodParser(char* method);
bool booleanParser(char* boolean);

//Constraints
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

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);

	//if filename is NULL
	if (!filename) {
		spLoggerDestroy();
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	FILE * fp = fopen(filename, SP_CONFIG_OPEN_MODE);
	if (!fp) { //If opening failed
		spLoggerDestroy();
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}

	SPConfig config = (SPConfig) malloc(sizeof(struct sp_config_t));
	if (!config) { //If allocation failed
		fclose(fp);
		spLoggerDestroy();
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	//setting default values
	setConfigInitialValues(config);

	// going over the file
	int charNum, lineNum = 0;
	char currChar, *line = (char*) calloc(MAX_STRING_SIZE + 1, sizeof(char)); //+1 for null char
	if (!line) { //If allocation failed
		fclose(fp);
		spConfigDestroy(config);
		spLoggerDestroy();
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	while (!feof(fp)) {
		int lineLength = (int) strlen(line);
		memset(line, '\0', lineLength);
		charNum = 0;
		lineNum++;
		do {
			currChar = fgetc(fp);
			line[charNum++] = currChar;
		} while (currChar != '\n' && !feof(fp));
		line[--charNum] = '\0'; // remove last character (newline or eof)
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

		if (strcmp(variable, "") == 0) { //If allocation failed
			free(line);
			fclose(fp);
			spConfigDestroy(config);
			spLoggerDestroy();
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}

		value = getVariableValue(line);
		if (strcmp(value, "") == 0) { //If allocation failed
			free(line);
			free(variable);
			fclose(fp);
			spConfigDestroy(config);
			spLoggerDestroy();
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}

		if (!mapVarAndValToConfig(config, variable, value, filename, lineNum,
				msg)) {
			freeBeforeExit(config, fp, line, variable, value);
			return NULL;
		}
	}

	fclose(fp);
	free(line);

	//Check if variables without default values have been set
	if (!allVariablesSet(config, filename, lineNum, msg)) {
		spConfigDestroy(config);
		spLoggerDestroy();
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

int spConfigGetLoggerLevel(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spLoggerLevel;
}

char* spConfigGetLoggerFilename(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	char* ret = (char*) calloc(strlen(config->spLoggerFilename) + 1,
			sizeof(char));
	if (!ret) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	strcpy(ret, config->spLoggerFilename);
	return ret;
}

//todo remove?
int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (!config) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
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

bool spConfigExpected(SPConfig config, char* spImagesDirectory,
		char* spImagesPrefix, char* spImagesSuffix, int spNumOfImages,
		int spPCADimension, char* spPCAFilename, int spNumOfFeatures,
		bool spExtractionMode, int spNumOfSimilarImages,
		SP_CONFIG_SPLIT_METHOD spKDTreeSplitMethod, int spKNN,
		bool spMinimalGUI, int spLoggerLevel, char* spLoggerFilename) {
	bool toReturn = true;
	if (strcmp(config->spImagesDirectory, spImagesDirectory) != 0) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spImagesDirectory. value: %s, expected: %s\n",
				config->spImagesDirectory, spImagesDirectory);
	}
	if (strcmp(config->spImagesPrefix, spImagesPrefix) != 0) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spImagesPrefix. value: %s, expected: %s\n",
				config->spImagesPrefix, spImagesPrefix);
	}
	if (strcmp(config->spImagesSuffix, spImagesSuffix) != 0) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spImagesSuffix. value: %s, expected: %s\n",
				config->spImagesSuffix, spImagesSuffix);
	}
	if (config->spNumOfImages != spNumOfImages) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spNumOfImages. value: %d, expected: %d\n",
				config->spNumOfImages, spNumOfImages);
	}
	if (config->spPCADimension != spPCADimension) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spPCADimension. value: %d, expected: %d\n",
				config->spPCADimension, spPCADimension);
	}
	if (strcmp(config->spPCAFilename, spPCAFilename) != 0) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spPCAFilename. value: %s, expected: %s\n",
				config->spPCAFilename, spPCAFilename);
	}
	if (config->spNumOfFeatures != spNumOfFeatures) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spNumOfFeatures. value: %d, expected: %d\n",
				config->spNumOfFeatures, spNumOfFeatures);
	}
	if (config->spExtractionMode != spExtractionMode) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spExtractionMode. value: %d, expected: %d\n",
				config->spExtractionMode, spExtractionMode);
	}
	if (config->spNumOfSimilarImages != spNumOfSimilarImages) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spNumOfSimilarImages. value: %d, expected: %d\n",
				config->spNumOfSimilarImages, spNumOfSimilarImages);
	}
	if (config->spKDTreeSplitMethod != spKDTreeSplitMethod) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spKDTreeSplitMethod. value: %d, expected: %d\n",
				config->spKDTreeSplitMethod, spKDTreeSplitMethod);
	}
	if (config->spKNN != spKNN) {
		toReturn = false;
		printf("COMPARISON FAILED at variable spKNN. value: %d, expected: %d\n",
				config->spKNN, spKNN);
	}
	if (config->spMinimalGUI != spMinimalGUI) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spMinimalGUI. value: %d, expected: %d\n",
				config->spMinimalGUI, spMinimalGUI);
	}
	if (config->spLoggerLevel != spLoggerLevel) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spLoggerLevel. value: %d, expected: %d\n",
				config->spLoggerLevel, spLoggerLevel);
	}
	if (strcmp(config->spLoggerFilename, spLoggerFilename) != 0) {
		toReturn = false;
		printf(
				"COMPARISON FAILED at variable spLoggerFilename. value: %s, expected: %s\n",
				config->spLoggerFilename, spLoggerFilename);
	}
	return toReturn;
}

/////////////////////////////////
// General auxiliary functions //
/////////////////////////////////

bool isValidConfigLine(char* line) {
// s*cc*s*=s*ccs*\0;
// s = whitespace, c = non-whitespace
// *=0 or more (regular expressions)
	int i = 0;
	while (isspace(line[i])) {
		i++;
	}
	if (line[i] == '\0' || line[i] == '#') { //empty or comments only
		return true;
	}
	char variable[strlen(line) + 1];
	for (int j = 0; !isspace(line[i]) && line[i] != '='; j++, i++) {
		if (line[i] == '\0') {
			return false;
		}
		variable[j] = line[i];
	}
	if (strlen(variable) == 0) {
		return false;
	}
	if (line[i] != '=') {
		while (isspace(line[i])) {
			i++;
		}
	}
	if (line[i] == '\0' || line[i] == '#') {
		return false;
	}
	i++;
// line[i] = '='
	while (isspace(line[i])) {
		i++;
	}
	char value[strlen(line) + 1];
	for (int j = 0; !isspace(line[i]) && line[i] != '\0'; j++, i++) {
		value[j] = line[i];
	}
	if (strlen(value) == 0) {
		return false;
	}
	while (isspace(line[i])) {
		i++;
	}
	return (line[i] == '\0' || line[i] == '#');
}

void setConfigInitialValues(SPConfig config) {
// setting default values
	config->spImagesDirectory = NULL; // Just to check if this has been set to a valid value
	config->spImagesPrefix = NULL; // Just to check if this has been set to a valid value
	config->spImagesSuffix = NULL; // Just to check if this has been set to a valid value
	config->spNumOfImages = 0; // Just to check if this has been set to a valid value
	config->spPCADimension = spPCADimension_DEFAULT;
	config->spPCAFilename = (char *) calloc(strlen(spPCAFilename_DEFAULT) + 1,
			sizeof(char));
	strcpy(config->spPCAFilename, spPCAFilename_DEFAULT);
	config->spNumOfFeatures = spNumOfFeatures_DEFAULT;
	config->spExtractionMode = spExtractionMode_DEFAULT;
	config->spNumOfSimilarImages = spNumOfSimilarImages_DEFAULT;
	config->spKDTreeSplitMethod = spKDTreeSplitMethod_DEFAULT;
	config->spKNN = spKNN_DEFAULT;
	config->spMinimalGUI = spMinimalGUI_DEFAULT;
	config->spLoggerLevel = spLoggerLevel_DEFAULT;
	config->spLoggerFilename = (char *) calloc(
			strlen(spLoggerFilename_DEFAULT) + 1, sizeof(char));
	strcpy(config->spLoggerFilename, spLoggerFilename_DEFAULT);
}

/**
 * This function maps a variable and a value into a config and prints in case something goes wrong.
 * It is an auxiliary function and is called from spConfigCreate only.
 *
 * @param config - the configuration structure
 * @param variable - a string represents the variable's name
 * @param value - a string represents the variable's value
 * @param filename - the name of the file
 * @param lineNum - the number of the line
 * @param msg - a pointer in which the msg returned by the function is stored
 * @param spImagesDirectorySet - a pointer which value is true if the variable set is spImagesDirectory
 * @param spImagesPrefixSet - a pointer which value is true if the variable set is spImagesPrefix
 * @param spImagesSuffixSet - a pointer which value is true if the variable set is spImagesSuffix
 * @param spNumOfImagesSet - a pointer which value is true if the variable set is spNumOfImages
 *
 * @return true if the mapping is successful, false otherwise (an error occurs).
 *
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_INVALID_BOOLEAN - if a line in the config file contains invalid boolean
 * - SP_CONFIG_INVALID_SP_CONFIG_INVALID_SPLIT_METHOD - if a line in the config file contains invalid split method
 * - SP_CONFIG_INVALID_LINE - if a line in the config file has a variable that doesn't exist
 * - no change - in case of success (will be set in spConfigCreate)
 */
bool mapVarAndValToConfig(SPConfig config, char* variable, char* value,
		const char* filename, int lineNum, SP_CONFIG_MSG* msg) {
	if (strcmp(variable, "spImagesDirectory") == 0) {
		if (spImagesDirectoryConstraint(value)) {
			free(config->spImagesDirectory);
			config->spImagesDirectory = (char*) calloc(strlen(value) + 1,
					sizeof(char));
			strcpy(config->spImagesDirectory, value);
		} else {
			MAP_CASE_STRING_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spImagesPrefix") == 0) {
		if (spImagesPrefixConstraint(value)) {
			free(config->spImagesPrefix);
			config->spImagesPrefix = (char*) calloc(strlen(value) + 1,
					sizeof(char));
			strcpy(config->spImagesPrefix, value);
		} else {
			MAP_CASE_STRING_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spImagesSuffix") == 0) {
		if (spImagesSuffixConstraint(value)) {
			free(config->spImagesSuffix);
			config->spImagesSuffix = (char*) calloc(strlen(value) + 1,
					sizeof(char));
			strcpy(config->spImagesSuffix, value);
		} else {
			MAP_CASE_STRING_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spNumOfImages") == 0) {
		if (!isInt(value)) {
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return false;
		} else if (spNumOfImagesConstraint(atoi(value))) {
			config->spNumOfImages = atoi(value);
		} else {
			MAP_CASE_INTEGER_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spPCADimension") == 0) {
		if (!isInt(value)) {
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return false;
		} else if (spPCADimensionConstraint(atoi(value))) {
			config->spPCADimension = atoi(value);
		} else {
			MAP_CASE_INTEGER_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spPCAFilename") == 0) {
		if (spPCAFilenameConstraint(value)) {
			free(config->spPCAFilename);
			config->spPCAFilename = (char*) calloc(strlen(value) + 1,
					sizeof(char));
			strcpy(config->spPCAFilename, value);
		} else {
			MAP_CASE_STRING_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spNumOfFeatures") == 0) {
		if (!isInt(value)) {
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return false;
		} else if (spNumOfFeaturesConstraint(atoi(value))) {
			config->spNumOfFeatures = atoi(value);
		} else {
			MAP_CASE_INTEGER_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spExtractionMode") == 0) {
		if (spExtractionModeConstraint(value)) {
			config->spExtractionMode = booleanParser(value);
		} else {
			MAP_CASE_BOOLEAN_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spNumOfSimilarImages") == 0) {
		if (!isInt(value)) {
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return false;
		} else if (spNumOfSimilarImagesConstraint(atoi(value))) {
			config->spNumOfSimilarImages = atoi(value);
		} else {
			MAP_CASE_INTEGER_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spKDTreeSplitMethod") == 0) {
		if (spKDTreeSplitMethodConstraint(value)) {
			config->spKDTreeSplitMethod = spKDTreeSplitMethodParser(value);
		} else {
			MAP_CASE_SPLIT_METHOD_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spKNN") == 0) {
		if (!isInt(value)) {
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return false;
		} else if (spKNNConstraint(atoi(value))) {
			config->spKNN = atoi(value);
		} else {
			MAP_CASE_INTEGER_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spMinimalGUI") == 0) {
		if (spMinimalGUIConstraint(value)) {
			config->spMinimalGUI = booleanParser(value);
		} else {
			MAP_CASE_BOOLEAN_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spLoggerLevel") == 0) {
		if (!isInt(value)) {
			*msg = SP_CONFIG_INVALID_LINE;
			printf(INVALID_LINE_STRING, filename, lineNum);
			return false;
		} else if (spLoggerLevelConstraint(atoi(value))) {
			config->spLoggerLevel = atoi(value);
		} else {
			MAP_CASE_INTEGER_CONSTRAINT_NOT_MET
		}
	} else if (strcmp(variable, "spLoggerFilename") == 0) {
		if (spPCAFilenameConstraint(value)) {
			free(config->spLoggerFilename);
			config->spLoggerFilename = (char*) calloc(strlen(value) + 1,
					sizeof(char));
			strcpy(config->spLoggerFilename, value);
		} else {
			MAP_CASE_STRING_CONSTRAINT_NOT_MET
		}
	} else {
		// variable is invalid
		*msg = SP_CONFIG_INVALID_LINE;
		printf(INVALID_LINE_STRING, filename, lineNum);
		return false;
	}
	free(variable);
	free(value);
	return true;
}

bool allVariablesSet(SPConfig config, const char* filename, int lineNum,
		SP_CONFIG_MSG* msg) {
//Check if variables without default values has been set
	if (!config->spImagesDirectory) {
		*msg = SP_CONFIG_MISSING_DIR;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum,
				"spImagesDirectory");
		return false;
	}
	if (!config->spImagesPrefix) {
		*msg = SP_CONFIG_MISSING_PREFIX;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum, "spImagesPrefix");
		return false;
	}
	if (!config->spImagesSuffix) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum, "spImagesSuffix");
		return false;
	}
	if (config->spNumOfImages == 0) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		printf(PARAMETER_NOT_SET_STRING, filename, lineNum, "spNumOfImages");
		return false;
	}
	return true;
}

void freeBeforeExit(SPConfig config, FILE* file, char* line, char* variable,
		char* value) {
	fclose(file);
	spConfigDestroy(config);
	spLoggerDestroy();
	free(line);
	free(variable);
	free(value);
}

//////////////////
// Line Parsers //
//////////////////

char* getVariableName(const char* line) {
// We assume line is valid
// s*cc*s*=s*ccs*\0;
// s = whitespace, c = non-whitespace
// *=0 or more (regular expressions)
	int i = 0;
	while (isspace(line[i])) {
		i++;
	}
	if (line[i] == '\0' || line[i] == '#') { //empty or comments only
		return NULL;
	}
	char* variable = (char*) calloc(MAX_STRING_SIZE + 1, sizeof(char)); //+1 for null char
	if (!variable) {
		return "";
	}
	for (int j = 0; !isspace(line[i]) && line[i] != '='; j++, i++) {
		variable[j] = line[i];
	}
	return variable;
}

char* getVariableValue(const char* line) {
// s*cc*s*=s*ccs*\0;
// s = whitespace, c = non-whitespace
// *=0 or more (regular expressions)
	int i = 0;
	while (isspace(line[i])) {
		i++;
	}
	if (line[i] == '\0' || line[i] == '#') { //empty or comments only
		return NULL;
	}
	for (int j = 0; !isspace(line[i]) && line[i] != '='; j++, i++) {
		// variable
	}
	if (line[i] != '=') {
		while (isspace(line[i])) {
			i++;
		}
	}
	if (line[i] == '\0' || line[i] == '#') {
		return false;
	}
	i++;
// line[i] = '='
	while (isspace(line[i])) {
		i++;
	}
	char* value = (char*) calloc(MAX_STRING_SIZE + 1, sizeof(char)); //+1 for null char
	if (!value) {
		return "";
	}
	for (int j = 0; !isspace(line[i]) && line[i] != '\0'; j++, i++) {
		value[j] = line[i];
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
	for (int i = 0; i < (int) strlen(str); i++) {
		if (str[i] == ' ') {
			return false;
		}
	}
	return true;
}

bool positiveNum(int num) {
	return num > 0;
}

bool isInt(char* numString) {
	for (int i = 0; i < (int) strlen(numString); i++) {
		if (!isdigit(numString[i])) {
			return false;
		}
	}
	return true;
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

////////////////////
// String Parsers //
////////////////////

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

