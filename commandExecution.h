/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for command execution and termination functions
*/

void status(int exitStatus);

void changeDirectory(struct command* command);

void redirectInput(struct command* command, int* savedIn, bool* restoreIn);

void redirectOutput(struct command* command, int* savedOut, bool* restoreOut);

void restoreIOStreams(bool restoreIn, int savedIn, bool restoreOut, int savedOut);

void executeCommand(struct command* command, struct dynamicArray* backgroundPids, int* lastStatus);

void terminateBackgroundProcesses(struct dynamicArray* backgroundPids);
