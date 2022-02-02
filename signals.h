/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Header file for signal handling functions
*/

/*
* A signal handler for the SIGTSTP signal. This signal handler will check the forgroundOnlyMode global
* variable - if it is 1, the signal handler will change its value to 0; if it is 0, the signal handler
* will change its value to 1. A forgroundOnlyMode value of 1 signifies that the shell is in foreground
* only mode. A foregroundOnlyMode value of 0 signifies that the shell is not in foreground only mode
*/
void handle_SIGTSTP(int signo);

/*
* Populates the sigaction struct used to register the signal handler for the SIGTSTP signal
*/
void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action);

/*
* Populates the sigaction struct used to register the signal handler associated with ignoring signals
*/
void fill_ignore_action(struct sigaction* ignore_action);
