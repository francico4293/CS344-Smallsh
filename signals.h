/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for signal handling functions
*/

void handle_SIGINT(int signo);

void register_SIGINT_action(struct sigaction* SIGINT_action);

void register_ignoreAction(struct sigaction* ignoreAction);
