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
static bool basicConfigTest() {
	SP_LOGGER_MSG loggerMsg = spLoggerCreate("stdout", 3);
	ASSERT_TRUE(loggerMsg == SP_LOGGER_SUCCESS);
	SP_CONFIG_MSG* configMsg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	spConfigCreate("basicConfigTest1.txt", configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_INVALID_INTEGER);
	SPConfig config2 = spConfigCreate("basicConfigTest2.txt", configMsg);
	ASSERT_TRUE(*configMsg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(
			spConfigExpected(config2, "C:/", "picture", ".jpg", 26, 28, "ppasnapa.txt", 161, false, 2, 2, 51, true, 1, "stdout"));
	spLoggerDestroy();
	spConfigDestroy(config2);
	free(configMsg);
	return true;
}

int main() {
	RUN_TEST(basicConfigTest);
	return 0;
}
