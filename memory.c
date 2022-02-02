/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Function associated with memory cleanup
*/
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include "dynamicArray.h"
#include "parser.h"

/*
* Releases all memory allocated for the command struct and for use with the attributes of
* the command struct
*/
void cleanupMemory(struct command* command) {
	int index = 0;

	// iterate over each character pointer in the argv array and release the memory allocated
	// for each one
	while (command->argv[index]) {
		free(command->argv[index]);
		index++;
	}
	// release the memory allocated for the argv array itself
	free(command->argv);

	// check if newInput is not NULL
	if (command->newInput) {
		// release the memory allocated for newInput
		free(command->newInput);
	}

	// check if newOutput is not NULL
	if (command->newOutput) {
		// release the memory allocated for newOutput
		free(command->newOutput);
	}

	// release the memory allocated for the command struct
	free(command);
}

/*
* Releases all memory allocated for the command struct and for use with the the attributes of
* the command struct. Terminates any open background processes. Releases memory allocated for
* the dynamic array used to track runnning background processes
*/
void cleanupMemoryAndExit(struct command* command, struct dynamicArray* backgroundPids) {
	// declare a variable used to store a process id
	pid_t backgroundPid;
	// declare a variable used to store the status of a process
	int backgroundPidStatus;

	// release memory allocated for the command struct and its members
	cleanupMemory(command);

	// iterate over each element in the backgroundPids array
	for (int index = 0; index < backgroundPids->size; index++) {
		// if this statement returns zero, then the background process is still running
		if ((backgroundPid = waitpid(backgroundPids->staticArray[index], &backgroundPidStatus, WNOHANG)) == 0) {
			// kill the running background process
			kill(backgroundPids->staticArray[index], SIGKILL);
		}
	}

	// free memory allocated for the static array member in the dynamic array struct
	free(backgroundPids->staticArray);
	// free memory allocated for the dynamic array struct
	free(backgroundPids);
}
