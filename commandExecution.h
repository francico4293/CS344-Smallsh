/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for command execution and termination functions
*/

void status(int exitStatus);

void executeCommand(struct command* command, struct dynamicArray* backgroundPids, int* lastStatus);

void terminateBackgroundProcesses(struct dynamicArray* backgroundPids);
