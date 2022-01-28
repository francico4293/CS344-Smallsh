/*
* Author: Colin Francis
* ONID: francico
* Description: Functions associated with signal handling
*/
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "globalVariables.h"

void handle_SIGTSTP(int signo) {
	if (!flag) {
		write(STDOUT_FILENO, "SIGTSTP\n", 8);
	}
}

void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action) {
	SIGTSTP_action->sa_handler = handle_SIGTSTP;
	SIGTSTP_action->sa_flags = SA_RESTART;
	sigfillset(&(SIGTSTP_action->sa_mask));
	sigdelset(&(SIGTSTP_action->sa_mask), SIGINT);
}

void fill_ignoreAction(struct sigaction* ignoreAction) {
	ignoreAction->sa_handler = SIG_IGN;
	ignoreAction->sa_flags = SA_RESTART;
	sigfillset(&(ignoreAction->sa_mask));
}
