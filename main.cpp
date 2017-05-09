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
	return 0;
}
