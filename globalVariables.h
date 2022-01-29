/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for global variable for sharing information between signal handlers and main program
*/
#include <setjmp.h>

sigjmp_buf mark;

volatile sig_atomic_t foregroundOnlyMode;
