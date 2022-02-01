/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
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
#include <signal.h>
#include "dynamicArray.h"
#include "parser.h"
#include "signals.h"
#include "globalVariables.h"
#include "memory.h"

/*
* Prints the exit or termination status of a process based on the value in exitStatus
*/
void status(int exitStatus) {
	// if WIFEXITED returns true, then the process was terminated normally
	if (WIFEXITED(exitStatus)) {
		// get and print the exit status via WEXITSTATUS 
		printf("exit value %d\n", WEXITSTATUS(exitStatus));
		fflush(stdout);  // flush stdout
	}
	// if WIFEXITED didn't return true, then the process was terminated abnormally
	else {
		// get and print the signal number that caused the process to terminate via WTERMSIG
		printf("terminated by signal %d\n", WTERMSIG(exitStatus));
		fflush(stdout);  // flush stdout
	}
}

void changeDirectory(struct command* command) {
	// declare a character array of size PATH_MAX
	char currentWorkingDir[PATH_MAX];
	// get the directory specified in the HOME environment variable and store it in a variable named home
	char* home = getenv("HOME");

	// argv[1] is NULL then the user only entered "cd"
	if (!command->argv[1]) {
		// chamge to the directory specified in the HOME environment variable
		chdir(home);
		return;  // exit the function
	}

	// get the current working directory and store in the currentWorkingDir variable
	getcwd(currentWorkingDir, PATH_MAX);
	// if home is detected in the path specified by the user then the user is specifying an absolute path
	if (strncmp(command->argv[1], home, strlen(home)) == 0) {
		// since the path specified by the user is absolute, use chdir to change to the directory specified
		// by the absolute path
		if (chdir(command->argv[1]) == -1) {
			// in the event that chdir returns -1, then the directory was not found - display an error message
			// to the user
			printf("%s: No such file or directory\n", command->argv[1]);
			// flush stdout
			fflush(stdout);
		}
	}
	// if home is not detected in the path specified by the user then the user is specifying a relative path
	else {
		// to change to the correct directory, we need to concatenate the path in currentWorkingDir with the path specified 
		// by the user and available in command->argv[1] - allocate memory large enough to hold the full path that must be 
		// built
		char* path = (char*)malloc((strlen(currentWorkingDir) + strlen("/") + strlen(command->argv[1]) + 1) * sizeof(char));

		// copy the characters in currentWorkingDir into the memory segment allocated for path
		strcpy(path, currentWorkingDir);
		// concatenate path and "/"
		strcat(path, "/");
		// concatenate path and the path specified by the user which is available in command->argv[1]
		strcat(path, command->argv[1]);
		
		// change to the directory specified by path
		if (chdir(path) == -1) {
			// in the event that chdir returns -1, then the directory was not found - display an error message
			// to the user
			printf("%s: No such file or directory\n", command->argv[1]);
			// flush stdout
			fflush(stdout);
		}

		// free the memory allocated for path
		free(path);
	}
}

/*
* Redirects the input stream from stdin to the input stream specified by the user. In the event that the process being run is
* a background process, the input stream will be redirected to "/dev/null"
*/
void redirectInput(struct command* command, int* savedIn, bool* restoreIn) {
	int targetInFD;

	*savedIn = dup(STDIN_FILENO);
	*restoreIn = true;

	if (command->inputRedirect) {
		targetInFD = open(command->newInput, O_RDONLY);
	}
	else {
		targetInFD = open("/dev/null", O_RDONLY);
	}

	if (targetInFD == -1) {
		printf("%s: No such file or directory\n", command->newInput);
		fflush(stdout);
		exit(1);
	}

	int inResult = dup2(targetInFD, STDIN_FILENO);
	if (inResult == -1) {
		perror("dup2");
		exit(1);
	}
}

void redirectOutput(struct command* command, int* savedOut, bool* restoreOut) {
	int targetOutFD;

	*savedOut = dup(STDOUT_FILENO);
	*restoreOut = true;

	if (command->outputRedirect) {
		targetOutFD = open(command->newOutput, O_WRONLY | O_CREAT | O_TRUNC, 0640);
	}
	else {
		targetOutFD = open("/dev/null", O_WRONLY);
	}
	
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

void terminateBackgroundProcesses(struct dynamicArray* backgroundPids) {
	pid_t backgroundPid;
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

void executeCommand(struct command* command, struct dynamicArray* backgroundPids, int* lastStatus) {
	int childStatus;
	pid_t spawnPid;
	bool restoreOut = false;
	int savedOut;
	bool restoreIn = false;
	int savedIn;
	
	if (strcmp(command->argv[0], "status") == 0) {
		status(*lastStatus);
		return;
	}

	if (strcmp(command->argv[0], "cd") == 0) {
		changeDirectory(command);
		return;
	}

	if (strcmp(command->argv[0], "exit") == 0) {
		cleanupMemoryAndExit(command, backgroundPids);
		exit(0);
	}

	spawnPid = fork();
	if (spawnPid == -1) {
		perror("fork");
		exit(1);
	}
	else if (spawnPid == 0) {
		if (command->inputRedirect || command->backgroundProcess) {
			redirectInput(command, &savedIn, &restoreIn);
		}

		if (command->outputRedirect || command->backgroundProcess) {
			redirectOutput(command, &savedOut, &restoreOut);
		}

		if (!command->backgroundProcess || foregroundOnlyMode) {
			signal(SIGINT, SIG_DFL);
		}

		struct sigaction ignore_action = { 0 };
		fill_ignore_action(&ignore_action);
		sigaction(SIGTSTP, &ignore_action, NULL);

		execvp(command->pathName, command->argv);

		restoreIOStreams(restoreIn, savedIn, restoreOut, savedOut);

		printf("%s: No such file or directory\n", command->pathName);
		fflush(stdout);

		cleanupMemoryAndExit(command, backgroundPids);

		exit(1);
	}
	else {
		if (!command->backgroundProcess || foregroundOnlyMode) {
			spawnPid = waitpid(spawnPid, &childStatus, 0);
			*lastStatus = childStatus;

			if (WIFSIGNALED(childStatus) && WTERMSIG(childStatus) == 2) {
				printf("terminated by signal %d\n", WTERMSIG(childStatus));
				fflush(stdout);
			}
		}
		else {
			append(backgroundPids, spawnPid);
			printf("background pid is %d\n", spawnPid);
			fflush(stdout);
		}
	}

	terminateBackgroundProcesses(backgroundPids);
}
