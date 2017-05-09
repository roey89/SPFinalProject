/*
 * main_aux.cpp
 *
 *  Created on: May 9, 2017
 *      Author: Roey
 */

#include <cstdlib>
#include <cstring>

//C source files
extern "C" {
#include "SPConfig.h"
}

SPConfig createConfigFromCmd(int argc, char** argv) {
	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	SPConfig config;
	if (argc == 1) {
		config = spConfigCreate("spcbir.config", msg);
		if (*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(
					"The default configuration file spcbir.config couldn’t be open\n");
			free(msg);
			return NULL;
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "-c") == 0) {
			config = spConfigCreate(argv[2], msg);
			if (*msg == SP_CONFIG_SUCCESS) {
				free(msg);
				return config;
			}
			if (*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
				printf("The configuration file %s couldn’t be open\n", argv[2]);
			}
			free(msg);
			return NULL;
		}
	}
	printf("Invalid command line : use -c <config_filename>\n");
	free(msg);
	return NULL;
}
