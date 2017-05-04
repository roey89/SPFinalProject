/*
 * sp_config_unit_test_.c
 *
 *  Created on: May 4, 2017
 *      Author: Roey
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPConfig.h"

static bool basicConfigTest() {
	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	SPConfig config = spConfigCreate("basicConfigTest.txt", msg);
	return (strcmp(config->spImagesDirectory, "./images/") == 0)
			&& (strcmp(config->spImagesPrefix, "img") == 0)
			&& (strcmp(config->spImagesSuffix, ".png") == 0)
			&& (config->spNumOfImages == 17) && (config->spPCADimension == 20)
			&& (strcmp(config->spPCAFilename, "pca.yml") == 0)
			&& (config->spNumOfFeatures == 150)
			&& (config->spExtractionMode == true)
			&& (config->spNumOfSimilarImages == 5)
			&& (config->spKDTreeSplitMethod == MAX_SPREAD)
			&& (config->spKNN == 1) && (config->spMinimalGUI == false)
			&& (config->spLoggerLevel == 3)
			&& (strcmp(config->spLoggerFilename, "loggerfilefortest.txt") == 0);
}

int main() {
	RUN_TEST(basicConfigTest);
	return 0;
}
