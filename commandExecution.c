/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Functions associated with command excecution and termination
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include "dynamicArray.h"
#include "parser.h"
#include "signals.h"
#include "memory.h"

/*
* Prints the exit or termination status of a process based on the value in exitStatus
*/
void status(int exitStatus) {
	// if WIFEXITED returns true, then the process was terminated normally
	if (WIFEXITED(exitStatus)) {
		// get and print the exit status via WEXITSTATUS 
		printf("exit value %d\n", WEXITSTATUS(exitStatus));
		fflush(stdout);  // flush stdout
	}
	// if WIFEXITED didn't return true, then the process was terminated abnormally
	else {
		// get and print the signal number that caused the process to terminate via WTERMSIG
		printf("terminated by signal %d\n", WTERMSIG(exitStatus));
		fflush(stdout);  // flush stdout
	}
}

/*
* Changes the current working directory based on a user specified path
*/
void changeDirectory(struct command* command) {
	// declare a character array of size PATH_MAX (reference citation H)
	char currentWorkingDir[PATH_MAX];
	// get the directory specified in the HOME environment variable and store it in a variable named home
	char* home = getenv("HOME");

	// argv[1] is NULL then the user only entered "cd"
	if (!command->argv[1]) {
		// chamge to the directory specified in the HOME environment variable
		if (chdir(home) == -1) {
			// in the even that chdir fails to go home, display an error message to the user
			printf("%s: Unable to go to home directory");
			// flush stdout
			fflush(stdout);
		}
		return;  // exit the function
	}

	// get the current working directory and store in the currentWorkingDir variable
	getcwd(currentWorkingDir, PATH_MAX);
	// if home is detected in the path specified by the user then the user is specifying an absolute path
	if (strncmp(command->argv[1], home, strlen(home)) == 0) {
		// since the path specified by the user is absolute, use chdir to change to the directory specified
		// by the absolute path
		if (chdir(command->argv[1]) == -1) {
			// in the event that chdir returns -1, then the directory was not found - display an error message
			// to the user
			printf("%s: No such file or directory\n", command->argv[1]);
			// flush stdout
			fflush(stdout);
		}
	}
	// if home is not detected in the path specified by the user then the user is specifying a relative path
	else {
		// to change to the correct directory, we need to concatenate the path in currentWorkingDir with the path specified 
		// by the user and available in command->argv[1] - allocate memory large enough to hold the full path that must be 
		// built
		char* path = (char*)malloc((strlen(currentWorkingDir) + strlen("/") + strlen(command->argv[1]) + 1) * sizeof(char));

		// copy the characters in currentWorkingDir into the memory segment allocated for path
		strcpy(path, currentWorkingDir);
		// concatenate path and "/"
		strcat(path, "/");
		// concatenate path and the path specified by the user which is available in command->argv[1]
		strcat(path, command->argv[1]);
		
		// change to the directory specified by path
		if (chdir(path) == -1) {
			// in the event that chdir returns -1, then the directory was not found - display an error message
			// to the user
			printf("%s: No such file or directory\n", command->argv[1]);
			// flush stdout
			fflush(stdout);
		}

		// free the memory allocated for path
		free(path);
	}
}

/*
* Redirects the input stream from stdin to the input stream specified by the user. In the event that the process being run is
* a background process, the input stream will be redirected to "/dev/null"
*/
void redirectInput(struct command* command, int* savedIn, bool* restoreIn, struct dynamicArray* backgroundPids) {
	// declare a variable that will store the file descriptor associated with where the input file stream will be redirected
	int targetInFD;

	// set the integer at the address in savedIn equal to the value returned by dup(STDIN_FILENO), this will allow restoration of
	// stdin after the command has been executed (reference citation G)
	*savedIn = dup(STDIN_FILENO);
	// set the value at the address in restoreIn equal to true - this signifies that the original input stream must be restored after
	// command execution
	*restoreIn = true;

	// if command->inputRedirect is true then the user specified that they want the input redirected
	if (command->inputRedirect) {
		// get the file descriptor associated with where the input is being redirected from
		targetInFD = open(command->newInput, O_RDONLY);
	}
	// if command->inputRedirect wasn't true, then this function was called because the command being executed was flagged as a background
	// command
	else {
		// get the file descriptor associated with "/dev/null/
		targetInFD = open("/dev/null", O_RDONLY);
	}

	// if targetInFD is -1, then open failed
	if (targetInFD == -1) {
		// display an error message to the user
		printf("%s: No such file or directory\n", command->newInput);
		// flush stdout
		fflush(stdout);
		// cleanup memory and terminate any background processes
		cleanupMemoryAndExit(command, backgroundPids);
		// exit with status 1
		exit(1);
	}

	// redirect the input stream from STDIN_FILENO to the files descriptor in targetInFD
	int inResult = dup2(targetInFD, STDIN_FILENO);
	// if inResult is -1, then dup2 failed
	if (inResult == -1) {
		// display an error message to the user
		perror("dup2 failed");
		// cleanup memory and terminate any background processes
		cleanupMemoryAndExit(command, backgroundPids);
		// exit with status 1
		exit(1);
	}
}

/*
* Redirects the output stream from stdout to the output stream specified by the user. In the event that the process being run is
* a background process, the output stream will be redirected to "/dev/null"
*/
void redirectOutput(struct command* command, int* savedOut, bool* restoreOut, struct dynamicArray* backgroundPids) {
	// declare a variable that will store the file descriptor associated with where the output file stream will be redirected
	int targetOutFD;

	// set the integer at the address in savedOut equal to the value returned by dup(STDOUT_FILENO), this will allow restoration of
	// stdout after the command has been executed (reference citation G)
	*savedOut = dup(STDOUT_FILENO);
	// set the value at the address in restoreOut equal to true - this signifies that the original output stream must be restored after
	// command execution
	*restoreOut = true;

	// if command->outputRedirect is true then the user specified that they want the output redirected
	if (command->outputRedirect) {
		// get the file descriptor associated with where the output is being redirected to
		targetOutFD = open(command->newOutput, O_WRONLY | O_CREAT | O_TRUNC, 0640);
	}
	// if command->outputRedirect wasn't true, then this function was called because the command being executed was flagged as a background
	// command
	else {
		// get the file descriptor associated with "/dev/null/
		targetOutFD = open("/dev/null", O_WRONLY);
	}
	
	// if targetOutFD is -1, then open failed
	if (targetOutFD == -1) {
		// display an error message to the user
		printf("%s: No such file or directory\n", command->newOutput);
		// flush stdout
		fflush(stdout);
		// cleanup memory and terminate any background processes
		cleanupMemoryAndExit(command, backgroundPids);
		// exit with status 1
		exit(1);
	}

	// redirect the output stream from STDOUT_FILENO to the files descriptor in targetOutFD
	int outResult = dup2(targetOutFD, STDOUT_FILENO);
	// if outResult is -1, then dup2 failed
	if (outResult == -1) {
		// display an error message to the user
		perror("dup2 failed");
		// cleanup memory and terminate any background processes
		cleanupMemoryAndExit(command, backgroundPids);
		// exit with status 1
		exit(1);
	}
}

/*
* Restores I/O streams stored in savedIn and savedOut
* Reference citation G
*/
void restoreIOStreams(bool restoreIn, int savedIn, bool restoreOut, int savedOut) {
	// if restoreIn is true
	if (restoreIn) {
		// restore input stream back to STDIN_FILENO
		int inResult = dup2(savedIn, STDIN_FILENO);
		if (inResult == -1) {
			// display an error message to the user
			perror("dup2 failed");
			// exit with status 1
			exit(1);
		}
	}

	// if restoreOut is true
	if (restoreOut) {
		// restore output stream back to STDOUT_FILENO
		int outResult = dup2(savedOut, STDOUT_FILENO);
		if (outResult == -1) {
			// display an error message to the user
			perror("dup2 failed");
			// exit with status 1
			exit(1);
		}
	}
}

/*
* Terminates any background processes that have completed
*/
void terminateBackgroundProcesses(struct dynamicArray* backgroundPids) {
	// declare a variable used to hold a process id
	pid_t backgroundPid;
	// declare a variable used to hold the status of a background process
	int backgroundPidStatus;

	// iterate over each background pid in the backgroundPids array
	for (int index = 0; index < backgroundPids->size; index++) {
		// use waitpid function and WNOHANG option to invoke a non-blocking wait on the background pid associated with the
		// background pid at staticArray[index] - if waitpid returns a non-zero value, then the background process has 
		// completed
		if ((backgroundPid = waitpid(backgroundPids->staticArray[index], &backgroundPidStatus, WNOHANG)) != 0) {
			// display message with the pid of the process that has completed
			printf("background pid %d is done: ", backgroundPids->staticArray[index]);
			// flush stdout
			fflush(stdout);

			// display the exit status or the terminating signal of the process that terminated
			status(backgroundPidStatus);

			// delete the pid of the completed background process from the backgroundPids array
			delete(backgroundPids, index);
		}
	}
}

/*
* First checks if the command to be executed is one of three built-in commands - status, cd, or exit - and if so, the appropriate built-in
* command function is called to execute the built-in command. If the command to be executed is not a built-in command, then this function
* will fork of a child process which executes the user specified shell script
*/
void executeCommand(struct command* command, struct dynamicArray* backgroundPids, int* lastStatus) {
	// declare a variable used to store the exit or termination status of a child process
	int childStatus;
	// declare a variable used to store the pid of a forked child process
	pid_t spawnPid;
	// declare and initialize a variable used to signal if stdout stream should be restored
	bool restoreOut = false;
	// declare and initialize a variable used to store the file descriptor of the stdout stream to be
	// restored if restoration is necessary
	int savedOut;
	// declare and initialize a variable used to signal if stdin stream should be restored
	bool restoreIn = false;
	// declare and initialize a variable used to store the file descriptor of the stdin stream to be
	// restored if restoration is necessary
	int savedIn;
	
	// if "status" is found as the first element of the argv array
	if (strcmp(command->argv[0], "status") == 0) {
		// execute built-in "status" command
		status(*lastStatus);
		// return user back to command prompt
		return;
	}

	// if "cd" is found as the first element of the argv array
	if (strcmp(command->argv[0], "cd") == 0) {
		// execute built-in "cd" command
		changeDirectory(command);
		// return the user back to command prompt
		return;
	}

	// if "exit" is found as the first element of the argv array
	if (strcmp(command->argv[0], "exit") == 0) {
		// cleanup memory and terminate any background processes
		cleanupMemoryAndExit(command, backgroundPids);
		// exit with status 0
		exit(0);
	}

	// For the following code structure, reference citation F

	// fork a child process and store the return value in spawnPid variable
	spawnPid = fork();
	// if spawnPid is -1, then fork failed
	if (spawnPid == -1) {
		// display error message to the user
		perror("fork failed");
		// exit with status 1
		exit(1);
	}
	// if spawnPid is 0, then we are in the forked child process
	else if (spawnPid == 0) {
		// if inputRedirect is true or the command is flagged as being a background process
		if (command->inputRedirect || command->backgroundProcess) {
			// redirect input stream
			redirectInput(command, &savedIn, &restoreIn, backgroundPids);
		}

		// if output redirect is true or the command is flagged as being a background process
		if (command->outputRedirect || command->backgroundProcess) {
			// redirect output stream
			redirectOutput(command, &savedOut, &restoreOut, backgroundPids);
		}

		// if the command to be executed is not a background process or foregroundOnlyMode is set to 1, then
		// the command is going to be a foreground process and should terminate itself upon receiving SIGINT
		// from the OS - restore SIGINT back to it's default
		if (!command->backgroundProcess || foregroundOnlyMode) {
			signal(SIGINT, SIG_DFL);
		}

		// declare and initialize an empty sigaction struct named ignore_action
		struct sigaction ignore_action = { 0 };
		// populate the ignore_action struct
		fill_ignore_action(&ignore_action);
		// register the ignore_action struct with SIGTSTP signal because any foreground or background child
		// processes must ignore SIGTSTP
		sigaction(SIGTSTP, &ignore_action, NULL);

		// call execvp function to execute the user specified commmand by using the PATH variable to find the correct
		// shell script to be executed
		execvp(command->pathName, command->argv);

		// if we return, then execvp failed - restore all I/O streams
		restoreIOStreams(restoreIn, savedIn, restoreOut, savedOut);

		// display an error message to the user
		printf("%s: No such file or directory\n", command->pathName);
		// flush stdout
		fflush(stdout);

		// clean up memory and terminate any open background processes before exiting
		cleanupMemoryAndExit(command, backgroundPids);

		// exit with status 1
		exit(1);
	}
	// we are in the parent process
	else {
		// if the child process being executed is not a background process or if foregroundOnlyMode is set to 1,
		// then the child process will  be executed in the foreground and the parent must wait for the child to
		// terminate before continuing
		if (!command->backgroundProcess || foregroundOnlyMode) {
			// user waitpid with the spawnPid to wait for the child process to terminate
			spawnPid = waitpid(spawnPid, &childStatus, 0);
			// set the value of the address in lastStatus equal to the value in childStatus - this will be used to
			// determine the exit status or termination signal of the child process
			*lastStatus = childStatus;

			// if WIFSIGNALED is true and WTERMSIG is 2 then SIGINT was sent by the OS and the child process terminated
			// itself upon reception of SIGINT
			if (WIFSIGNALED(childStatus) && WTERMSIG(childStatus) == 2) {
				// display message about the termination signal to the user
				printf("terminated by signal %d\n", WTERMSIG(childStatus));
				// flush stdout
				fflush(stdout);
			}
		}
		// the child process is a background process and the parent process should continue and NOT wait
		else {
			// append the pid of the child process to the backgroundPids array in order to check when it has completed
			append(backgroundPids, spawnPid);
			// display a message about the pid of the child process to the user
			printf("background pid is %d\n", spawnPid);
			// flush stdout
			fflush(stdout);
		}
	}

	// check for any completed background processes and clean them up
	terminateBackgroundProcesses(backgroundPids);
}
