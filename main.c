/*
* Author: Colin Francis
* ONID: francico
* Descrtiption: Driver code and memory management for smallsh program
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "dynamicArray.h"
#include "parser.h"
#include "commandExecution.h"
#include "signals.h"

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

int main(void) {
	int lastStatus = 0;
	char* userInput = NULL;
	struct command* command = NULL;
	struct dynamicArray* backgroundPids = newDynamicArray();
	struct sigaction ignoreAction = { 0 };

	register_ignoreAction(&ignoreAction);

	while (true) {
		terminateBackgroundProcesses(backgroundPids);
		userInput = getCommandLineInput();

		// remove this later
		if (strcmp(userInput, "exit") == 0) {
			break;
		}

		command = parseUserInput(userInput);

		if (!command) {
			free(userInput);
			continue;
		}

		executeCommand(command, backgroundPids, &lastStatus);

		cleanupMemory(command);
	}

	free(userInput);
	free(backgroundPids->staticArray);
	free(backgroundPids);

	return EXIT_SUCCESS;
}
