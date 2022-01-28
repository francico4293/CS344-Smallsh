/*
* Author: Colin Francis
* ONID: francico
* Description: Functions associated with signal handling
*/
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void fill_ignoreAction(struct sigaction* ignoreAction) {
	ignoreAction->sa_handler = SIG_IGN;
	ignoreAction->sa_flags = SA_RESTART;
	sigfillset(&(ignoreAction->sa_mask));
}
