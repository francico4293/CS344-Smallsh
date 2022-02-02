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
#include <errno.h>
#include "globalVariables.h"

/*
* Function definition for foregroundOff signal handler
*/
void foregroundOff(int signo);

/*
* A signal handler for SIGTSTP signal - this signal handler will cause the shell to enter into foreground
* only mode where '&' associated with background processes will be ignored
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
* Populates the sigaction struct used to register the signal handler for the SIGTSTP signal
*/
void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action) {
	// set foregroundOn function as the initial signal handler
	SIGTSTP_action->sa_handler = foregroundOn;
	// allow for autmotatic restart of iterrupted system calls
	SIGTSTP_action->sa_flags = SA_RESTART;
	// set sa_mask attribute so that all signals are blocked while the signal handler is executing
	sigfillset(&(SIGTSTP_action->sa_mask));
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
