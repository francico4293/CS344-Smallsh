/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for signal handling functions
*/

void handle_SIGTSTP(int signo);

void fill_SIGTSTP_action(struct sigaction* SIGTSTP_action);

void fill_ignoreAction(struct sigaction* ignoreAction);
