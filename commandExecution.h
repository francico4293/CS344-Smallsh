/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Header file for command execution and termination functions
*/

/*
* Prints the exit or termination status of a process based on the value in exitStatus
*/
void status(int exitStatus);

/*
* Changes the current working directory based on a user specified path
*/
void changeDirectory(struct command* command);

/*
* Redirects the input stream from stdin to the input stream specified by the user. In the event that the process being run is
* a background process, the input stream will be redirected to "/dev/null"
*/
void redirectInput(struct command* command, int* savedIn, bool* restoreIn);

/*
* Redirects the output stream from stdout to the output stream specified by the user. In the event that the process being run is
* a background process, the output stream will be redirected to "/dev/null"
*/
void redirectOutput(struct command* command, int* savedOut, bool* restoreOut);

/*
* Restores I/O streams stored in savedIn and savedOut
*/
void restoreIOStreams(bool restoreIn, int savedIn, bool restoreOut, int savedOut);

/*
* Terminates any background processes that have completed
*/
void terminateBackgroundProcesses(struct dynamicArray* backgroundPids);

/*
* First checks if the command to be executed is one of three built-in commands - status, cd, or exit - and if so, the appropriate built-in
* command function is called to execute the built-in command. If the command to be executed is not a built-in command, then this function
* will fork of a child process which executes the user specified shell script
*/
void executeCommand(struct command* command, struct dynamicArray* backgroundPids, int* lastStatus);
