/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Functions associated with signal handling
*/
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "globalVariables.h"

/*
* A signal handler for the SIGTSTP signal. This signal handler will check the forgroundOnlyMode global
* variable - if it is 1, the signal handler will change its value to 0; if it is 0, the signal handler
* will change its value to 1. A forgroundOnlyMode value of 1 signifies that the shell is in foreground
* only mode. A foregroundOnlyMode value of 0 signifies that the shell is not in foreground only mode
*/
void handle_SIGTSTP(int signo) {
	// if foregroundOnlyMode == 0
	if (!foregroundOnlyMode) {
		// set foregroundOnlyMode to 1
		foregroundOnlyMode = 1;
		// display message that the shell is in foreground only mode
		write(STDOUT_FILENO, "\nEntering foreground-only mode (& is now ignored)\n", 50);
	}
	// if foregroundOnlyMode == 1
	else {
		// set forgroundOnlyMode to 0
		foregroundOnlyMode = 0;
		// display message that the shell is exiting foreground only mode
		write(STDOUT_FILENO, "\nExiting foreground-only mode\n", 30);
	}
}

/*
* Populates the sigaction struct used to register the signal handler for the SIGTSTP signal
*/
void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action) {
	// set handle_SIGTSTP function as the signal handler
	SIGTSTP_action->sa_handler = handle_SIGTSTP;
	// allow for autmotatic restart of iterrupted system calls
	SIGTSTP_action->sa_flags = SA_RESTART;
	// set sa_mask attribute so that all signals are blocked while the signal handler is executing
	sigfillset(&(SIGTSTP_action->sa_mask));
	// sigdelset(&(SIGTSTP_action->sa_mask), SIGINT);
}

/*
* Populates the sigaction struct used to register the signal handler associated with ignoring signals
*/
void fill_ignore_action(struct sigaction* ignore_action) {
	// set SIG_IGN function as the signal handler
	ignore_action->sa_handler = SIG_IGN;
	// allow for automatic restart of interrupted system calls
	ignore_action->sa_flags = SA_RESTART;
	// set sa_mask attribute so that all signals are blocked while the signal handler is executing
	sigfillset(&(ignore_action->sa_mask));
}
