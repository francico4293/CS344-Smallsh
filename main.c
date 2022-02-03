/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Driver code and signal handlers for smallsh program
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "dynamicArray.h"
#include "parser.h"
#include "commandExecution.h"
#include "signals.h"
#include "memory.h"

// A variable used to maintain a 0 or 1 value associated with the shell being in foreground
// only mode or not  1 = foregroundOnlyMode, 0 = !foregroundOnlyMode - this variable is used
// for the signal handlers to update as SIGTSTP is received
// Reference citation A
static volatile sig_atomic_t foregroundOnlyMode = 0;

/*
* Function definition for foregroundOff signal handler
* Reference citation A
*/
void foregroundOff(int signo);

/*
* A signal handler for SIGTSTP signal - this signal handler will cause the shell to enter into foreground
* only mode where '&' associated with background processes will be ignored
* Reference citation A
*/
void foregroundOn(int signo) {
	// save errno
	int saveErr = errno;

	// set flag indicating that the shell is in foreground only mode
	foregroundOnlyMode = 1;
	// display message to the user
	write(STDOUT_FILENO, "\nEntering foreground-only mode (& is now ignored)\n", 50);
	// update signal handler to foregroundOff so that next time SIGTSTP is received, we will exit foreground 
	// only mode
	signal(SIGTSTP, foregroundOff);

	// restore errno
	errno = saveErr;
}

/*
* A signal handler for SIGTSTP signal - this signal handler will cause the shell to exit foreground only
* mode where '&' while result in a process running in the background
* Reference citation A
*/
void foregroundOff(int signo) {
	// save errno
	int saveErr = errno;

	// set flag indicating that the shell is no longer in foreground only mode
	foregroundOnlyMode = 0;
	// display message to the user
	write(STDOUT_FILENO, "\nExiting foreground-only mode\n", 30);
	// update signal handler to foregroundOn so that next time SIGTSTP is received, we will enter foreground
	// only mode
	signal(SIGTSTP, foregroundOn);

	// restore errno
	errno = saveErr;
}

/*
* Driver code for smallsh program
*/
int main(void) {
	// declare and initialize a variable to store the exit status of the last foreground process
	int lastStatus = 0;
	// declare and initialize a variable to store the userInput returned after capturing command line
	// input from the user
	char* userInput = NULL;
	// declare and initialize a struct pointer to capture the return command struct pointer
	// that comes back from parsing user command line input
	struct command* command = NULL;
	// a flage used to signify if the shell is in foreground-only mode or not
	int foregroundFlag = 0;
	// create a dynamic array for use in tracking open background processes
	struct dynamicArray* backgroundPids = newDynamicArray();
	// declare and initialize sigaction structs ignore_action and SIGTSTP_action for use in
	// signal handling
	struct sigaction ignore_action = { 0 }, SIGTSTP_action = { 0 };
		
	// populate the ignore_action struct
	fill_ignore_action(&ignore_action);
	// register the ignore_action struct with SIGINT
	sigaction(SIGINT, &ignore_action, NULL);

	// populate the SIGTSTP_action struct
	fill_SIGTSTP_action(&SIGTSTP_action, foregroundOn);
	// register the SIGTSTP_action struct with SIGTSTP
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	// continue capture user input and executing the provided commands until the user exits the
	// program via the "exit" command
	while (true) {
		// if signal handler has set foreground only mode on, then update local flag (reference citation A)
		if (foregroundOnlyMode) {
			foregroundFlag = 1;
		}
		// if signal handler has set foreground only mode off, then update local flag (reference citation A)
		else {
			foregroundFlag = 0;
		}

		// display the command prompt ":" and await user input
		userInput = getCommandLineInput();

		// parse user input and capture the return command struct pointer
		command = parseUserInput(userInput);

		// if command is a NULL pointer then the user entered a blank line or a comment - ignore this
		if (!command) {
			// check for any completed background processes and clean them up
			terminateBackgroundProcesses(backgroundPids);
			// free memory segment in userInput
			free(userInput);
			// return user back to the command prompt ":" and await input
			continue;
		}

		// execute the command provided by the user
		executeCommand(command, backgroundPids, &lastStatus, foregroundFlag);

		// clean-up all allocated memory before returning the user back to the command prompt
		cleanupMemory(command);
	}

	return EXIT_SUCCESS;
}
