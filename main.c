/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Driver code for smallsh program
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
#include "memory.h"

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
	fill_SIGTSTP_action(&SIGTSTP_action);
	// register the SIGTSTP_action struct with SIGTSTP
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	// continue capture user input and executing the provided commands until the user exits the
	// program via the "exit" command
	while (true) {
		// display the command prompt ":" and await user input
		userInput = getCommandLineInput();

		// parse user input and capture the return command struct pointer
		command = parseUserInput(userInput);

		// if command is a NULL pointer then the user entered a blank line or a comment - ignore this
		if (!command) {
			// free memory segment in userInput
			free(userInput);
			// return user back to the command prompt ":" and await input
			continue;
		}

		// execute the command provided by the user
		executeCommand(command, backgroundPids, &lastStatus);

		// clean-up all allocated memory before returning the user back to the command prompt
		cleanupMemory(command);
	}

	return EXIT_SUCCESS;
}
