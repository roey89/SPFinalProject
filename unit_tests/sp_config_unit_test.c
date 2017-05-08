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
#include <malloc.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPConfig.h"

//Nothing should be printed
static bool basicConfigTest1() {
	SP_LOGGER_MSG loggerMsg = spLoggerCreate("stdout", 3);
	ASSERT_TRUE(loggerMsg == SP_LOGGER_SUCCESS);
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	spConfigCreate("basicConfigTest1.txt", configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_INTEGER);
	free(configMsg);
	return true;
}

static bool basicConfigTest2() {
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	SPConfig config = spConfigCreate("basicConfigTest2.txt", configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(
			spConfigExpected(config, "C:/", "picture", ".jpg", 26, 28, "ppasnapa.txt", 161, false, 2, 2, 51, true, 1, "stdout"));
	spConfigDestroy(config);
	free(configMsg);
	return true;
}

static bool invalidArgumentGettersTest() {
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	char* str = (char*) calloc(1024, sizeof(char));
	SPConfig config = NULL;
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	spConfigIsExtractionMode(config, configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	spConfigMinimalGui(config, configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	spConfigGetNumOfImages(config, configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	spConfigGetNumOfFeatures(config, configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	spConfigGetPCADim(config, configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	*configMsg = spConfigGetImagePath(str, config, 6);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	*configMsg = spConfigGetPCAPath(str, config);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_ARGUMENT);
	*configMsg = SP_CONFIG_ALLOC_FAIL;

	free(configMsg);
	free(str);
	return true;

}

static bool gettersTest() {
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	SPConfig config = spConfigCreate("basicConfigTest2.txt", configMsg);
	char* str = (char*) calloc(1024, sizeof(char));
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(
			spConfigExpected(config, "C:/", "picture", ".jpg", 26, 28, "ppasnapa.txt", 161, false, 2, 2, 51, true, 1, "stdout"));

	*configMsg = SP_CONFIG_ALLOC_FAIL;
	ASSERT_FALSE(spConfigIsExtractionMode(config, configMsg));
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	ASSERT_TRUE(spConfigMinimalGui(config, configMsg));
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	ASSERT_TRUE(spConfigGetNumOfImages(config, configMsg) == 26);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	ASSERT_TRUE(spConfigGetNumOfFeatures(config, configMsg) == 161);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	ASSERT_TRUE(spConfigGetPCADim(config, configMsg) == 28);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	*configMsg = spConfigGetImagePath(str, config, 6);
	ASSERT_TRUE(strcmp(str, "C:/picture6.jpg") == 0);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;
	*configMsg = spConfigGetPCAPath(str, config);
	ASSERT_TRUE(strcmp(str, "C:/ppasnapa.txt") == 0);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	*configMsg = SP_CONFIG_ALLOC_FAIL;

	spConfigDestroy(config);
	free(configMsg);
	free(str);
	return true;
}

int main() {
	RUN_TEST(basicConfigTest1);
	RUN_TEST(basicConfigTest2);
	RUN_TEST(invalidArgumentGettersTest);
	RUN_TEST(gettersTest);
	return 0;
}
