/*
 * main_aux.h
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPImageProc.h"

using namespace sp;

SPConfig createConfigFromCmd(int argc, char** argv);

SP_LOGGER_LEVEL intToLoggerLevel(int num);

void getQueryDirectoryFromUser(char* str);

bool createLoggerFromConfig(SPConfig config, SP_LOGGER_MSG* loggerMsg,
		SP_CONFIG_MSG* configMsg);

bool minimalGui(SPConfig config, int k, SP_CONFIG_MSG* configMsg,
		char* imagePath, int* k_nearest, ImageProc* proc, char* queryPath);

void storeFeatures(char* dir, SPPoint ** features, int imageIndex,
		int numberOfFeatures, int pointDim);

SPPoint ** loadFeatures(char* imageDir, int* numberofFeatures);

#endif /* MAIN_AUX_H_ */
