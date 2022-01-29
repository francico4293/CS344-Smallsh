/*
* Author: Colin Francis
* ONID: francico
* Descrtiption: Driver code for smallsh program
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include "dynamicArray.h"
#include "parser.h"
#include "commandExecution.h"
#include "signals.h"
#include "globalVariables.h"
#include "memory.h"

int main(void) {
	int lastStatus = 0;
	char* userInput = NULL;
	struct command* command = NULL;
	struct dynamicArray* backgroundPids = newDynamicArray();
	struct sigaction ignoreAction = { 0 }, SIGTSTP_action = { 0 };

	fill_ignoreAction(&ignoreAction);
	sigaction(SIGINT, &ignoreAction, NULL);

	fill_SIGTSTP_action(&SIGTSTP_action);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	while (true) {
		sigsetjmp(mark, 1);

		terminateBackgroundProcesses(backgroundPids);

		userInput = getCommandLineInput();

		command = parseUserInput(userInput);

		if (!command) {
			free(userInput);
			continue;
		}

		executeCommand(command, backgroundPids, &lastStatus);

		cleanupMemory(command);
	}

	return EXIT_SUCCESS;
}
