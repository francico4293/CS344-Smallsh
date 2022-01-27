/*
* Author: Colin Francis
* ONID: francico
* Description: Functions associated with command excecution and termination
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include "dynamicArray.h"
#include "parser.h"

void status(int exitStatus) {
	if (WIFEXITED(exitStatus)) {
		printf("exit value %d\n", WEXITSTATUS(exitStatus));
	}
	else {
		printf("terminated by signal %d\n", WTERMSIG(exitStatus));
	}
}

void executeCommand(struct command* command, struct dynamicArray* backgroundPids, int* lastStatus) {
	int childStatus;
	pid_t spawnPid;
	bool restoreOut = false;
	int savedOut;
	bool restoreIn = false;
	int savedIn;

	if (command->inputRedirect) {
		savedIn = dup(STDIN_FILENO);
		restoreIn = true;

		int targetInFD = open(command->newInput, O_RDONLY);
		if (targetInFD == -1) {
			perror("open");
			exit(1);
		}

		int inResult = dup2(targetInFD, STDIN_FILENO);
		if (inResult == -1) {
			perror("dup2");
			exit(1);
		}
	}

	if (command->outputRedirect) {
		savedOut = dup(STDOUT_FILENO);
		restoreOut = true;

		int targetOutFD = open(command->newOutput, O_WRONLY | O_CREAT | O_TRUNC, 0640);
		if (targetOutFD == -1) {
			perror("open");
			exit(1);
		}

		int outResult = dup2(targetOutFD, STDOUT_FILENO);
		if (outResult == -1) {
			perror("dup2");
			exit(1);
		}
	}

	if (strcmp(command->argv[0], "status") == 0) {
		status(*lastStatus);
		return;
	}

	spawnPid = fork();
	if (spawnPid == -1) {
		perror("fork");
		exit(1);
	}
	else if (spawnPid == 0) {
		execvp(command->pathName, command->argv);
		printf("%s: No such file or directory\n", command->pathName);
		fflush(stdout);
		exit(1);
	}
	else {
		if (!command->backgroundProcess) {
			spawnPid = waitpid(spawnPid, &childStatus, 0);
			*lastStatus = childStatus;

			if (restoreOut) {
				dup2(savedOut, STDOUT_FILENO);
			}

			if (restoreIn) {
				dup2(savedIn, STDIN_FILENO);
			}
		}
		else {
			printf("background pid is %d\n", spawnPid);
			fflush(stdout);
			append(backgroundPids, spawnPid);
		}
	}
}

void terminateBackgroundProcesses(struct dynamicArray* backgroundPids) {
	int backgroundPid;
	int backgroundPidStatus;

	for (int index = 0; index < backgroundPids->size; index++) {
		if ((backgroundPid = waitpid(backgroundPids->staticArray[index], &backgroundPidStatus, WNOHANG)) != 0) {
			printf("background pid %d is done: ", backgroundPids->staticArray[index]);
			fflush(stdout);

			status(backgroundPidStatus);

			delete(backgroundPids, index);
		}
	}
}