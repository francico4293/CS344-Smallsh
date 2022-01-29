/*
* Author: Colin Francis
* ONID: francico
* Description: Global variable for sharing information between signal handlers and main program
*/
#include <setjmp.h>
#include <signal.h>

sigjmp_buf mark;

volatile sig_atomic_t foregroundOnlyMode = 0;
