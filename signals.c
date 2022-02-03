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

/*
* Populates the sigaction struct used to register the signal handler for the SIGTSTP signal
*/
void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action, void(*handler)(int signo)) {
	// set foregroundOn function as the initial signal handler
	SIGTSTP_action->sa_handler = handler;
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
