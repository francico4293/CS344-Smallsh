/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Header file for signal handling functions

/*
* Populates the sigaction struct used to register the signal handler for the SIGTSTP signal
*/
void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action, void(*handler)(int signo));

/*
* Populates the sigaction struct used to register the signal handler associated with ignoring signals
*/
void fill_ignore_action(struct sigaction* ignore_action);
