/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Header file for signal handling functions
*/

// A variable used to maintain a 0 or 1 value associated with the shell being in foreground
// only mode or not - 1 = foregroundOnlyMode, 0 = !foregroundOnlyMode
static volatile sig_atomic_t foregroundOnlyMode;

/*
* A signal handler for SIGTSTP signal - this signal handler will cause the shell to enter into foreground
* only mode where '&' associated with background processes will be ignored
*/
void foregroundOn(int signo);

/*
* A signal handler for SIGTSTP signal - this signal handler will cause the shell to exit foreground only
* mode where '&' while result in a process running in the background
*/
void foregroundOff(int signo);

/*
* Populates the sigaction struct used to register the signal handler for the SIGTSTP signal
*/
void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action);

/*
* Populates the sigaction struct used to register the signal handler associated with ignoring signals
*/
void fill_ignore_action(struct sigaction* ignore_action);
