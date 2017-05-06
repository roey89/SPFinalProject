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
	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	SPConfig config = spConfigCreate("basicConfigTest.txt", msg);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_INTEGER);
	free(msg);
	ASSERT_TRUE(
			spConfigExpected(config, "./images/", "img", ".png", 17, 6, "pca.yml", 134, true, 3, 1, 1, false, 5, "loggerfilefortest.txt"));
	spConfigDestroy(config);
	return true;
}

int main() {
	RUN_TEST(basicConfigTest);
	return 0;
}
