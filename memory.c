/*
* Author: Colin Francis
* ONID: francico
* Description: Function associated with memory cleanup
*/
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"

void cleanupMemory(struct command* command) {
	int index = 0;

	while (command->argv[index]) {
		free(command->argv[index]);
		index++;
	}
	free(command->argv);

	if (command->newInput) {
		free(command->newInput);
	}

	if (command->newOutput) {
		free(command->newOutput);
	}

	free(command);
}
