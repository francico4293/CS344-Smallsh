/*
* Author: Colin Francis
* ONID: francico
* Description: Functions associated with signal handling
*/
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void handle_SIGINT(int signo) {
	write(STDOUT_FILENO, "Terminating\n", 25);
}

void register_SIGINT_action(struct sigaction* SIGINT_action) {
	SIGINT_action->sa_handler = handle_SIGINT;
	SIGINT_action->sa_flags = SA_RESTART;
	sigfillset(&(SIGINT_action->sa_mask));

	sigaction(SIGINT, SIGINT_action, NULL);
}

void register_ignoreAction(struct sigaction* ignoreAction) {
	ignoreAction->sa_handler = SIG_IGN;
	ignoreAction->sa_flags = SA_RESTART;
	sigfillset(&(ignoreAction->sa_mask));

	sigaction(SIGINT, ignoreAction, NULL);
}
