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
}

//C++ source files
#include "SPImageProc.h"
#include "main_aux.h"

int main(int argc, char** argv) {
	SPConfig config = createConfigFromCmd(argc, argv);
	if (!config) {
		return 0;
	}
//	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
//	char* loggerFileName = spConfigGetLoggerFilename(config, msg);
//	SP_LOGGER_MSG* loggerMsg = (SP_LOGGER_MSG*) malloc(sizeof(SP_LOGGER_MSG));
//	loggerMsg = spLoggerCreate(loggerFileName,
//			spConfigGetLoggerLevel(config, msg));
	spConfigDestroy(config);
	return 0;
}
