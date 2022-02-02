/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Global variable for sharing information between signal handlers and main program
*/
#include <signal.h>

// A global variable used to maintain a 0 or 1 value associated with the shell being in foreground
// only mode or not - 1 = foregroundOnlyMode, 0 = !foregroundOnlyMode
volatile sig_atomic_t foregroundOnlyMode = 0;
