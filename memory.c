/*
* Author: Colin Francis
* ONID: francico
* Description: Function associated with memory cleanup
*/
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include "dynamicArray.h"
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

void cleanupMemoryAndExit(struct command* command, struct dynamicArray* backgroundPids) {
	pid_t backgroundPid;
	int backgroundPidStatus;

	cleanupMemory(command);

	for (int index = 0; index < backgroundPids->size; index++) {
		if ((backgroundPid = waitpid(backgroundPids->staticArray[index], &backgroundPidStatus, WNOHANG)) == 0) {
			kill(backgroundPids->staticArray[index], SIGKILL);
			printf("Killed process with pid %d\n", backgroundPids->staticArray[index]);
		}
	}

	free(backgroundPids->staticArray);
	free(backgroundPids);
}
