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
#include <limits.h>
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

void changeDirectory(struct command* command) {
	char currentWorkingDir[PATH_MAX];
	char* home = getenv("HOME");

	if (!command->argv[1]) {
		chdir(home);
		return;
	}

	getcwd(currentWorkingDir, PATH_MAX);
	if (strncmp(command->argv[1], home, strlen(home)) == 0) {
		chdir(command->argv[1]);
	}
	else {
		char* path = (char*)malloc((strlen(currentWorkingDir) + strlen("/") + strlen(command->argv[1]) + 1) * sizeof(char));

		strcpy(path, currentWorkingDir);
		strcat(path, "/");
		strcat(path, command->argv[1]);
		chdir(path);

		free(path);
	}
}

void redirectInput(struct command* command, int* savedIn, bool* restoreIn) {
	*savedIn = dup(STDIN_FILENO);
	*restoreIn = true;

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

void redirectOutput(struct command* command, int* savedOut, bool* restoreOut) {
	*savedOut = dup(STDOUT_FILENO);
	*restoreOut = true;

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

void restoreIOStreams(bool restoreIn, int savedIn, bool restoreOut, int savedOut) {
	if (restoreOut) {
		dup2(savedOut, STDOUT_FILENO);
	}

	if (restoreIn) {
		dup2(savedIn, STDIN_FILENO);
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
		redirectInput(command, &savedIn, &restoreIn);
	}

	if (command->outputRedirect) {
		redirectOutput(command, &savedOut, &restoreOut);
	}

	if (strcmp(command->argv[0], "status") == 0) {
		status(*lastStatus);
		return;
	}

	if (strcmp(command->argv[0], "cd") == 0) {
		changeDirectory(command);
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

			restoreIOStreams(restoreIn, savedIn, restoreOut, savedOut);
		}
		else {
			printf("background pid is %d\n", spawnPid);
			fflush(stdout);
			append(backgroundPids, spawnPid);

			restoreIOStreams(restoreIn, savedIn, restoreOut, savedOut);
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
