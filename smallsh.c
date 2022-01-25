/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

struct command {
	char* pathName;
	char** argv;
	bool inputRedirect;  // <
	char* newInput;
	bool outputRedirect;  // >
	char* newOutput;
	bool backgroundProcess;  // &
};

char* getCommandLineInput(void) {
	char* userInput = NULL;
	char* inputBuffer = NULL;
	size_t length;
	ssize_t nread;

	printf(": ");
	nread = getline(&inputBuffer, &length, stdin);
	*(inputBuffer + strlen(inputBuffer) - 1) = '\0';

	userInput = (char*)malloc((strlen(inputBuffer) + 1) * sizeof(char));
	strcpy(userInput, inputBuffer);

	free(inputBuffer);

	return userInput;
}

void initializeCommandStruct(struct command* command, int numArgs) {
	command->inputRedirect = false;
	command->newInput = NULL;

	command->outputRedirect = false;
	command->newOutput = NULL;

	command->backgroundProcess = false;

	command->argv = (char**)malloc(numArgs * sizeof(char*));
	for (int index = 0; index < numArgs; index++) {
		command->argv[index] = NULL;
	}
}

char** appendArg(char* arg, char* argv[], int numArgs, int argvIndex) {
	int index = 0;
	char** newArgv = (char**)malloc((numArgs + 1) * sizeof(char*));

	argv[argvIndex] = (char*)malloc((strlen(arg) + 1) * sizeof(char));
	strcpy(argv[argvIndex], arg);

	while (index < numArgs) {
		newArgv[index] = argv[index];
		index++;
	}
	newArgv[index] = NULL;

	free(argv);
	return newArgv;
}

struct command* parseUserInput(char* userInput) {
	int numArgs = 2;
	int argvIndex = 0;
	char* token = NULL;
	char* savePtr = NULL;
	char* userInputCopy = (char*)malloc((strlen(userInput) + 1) * sizeof(char));
	struct command* command = (struct command*)malloc(sizeof(struct command));

	strcpy(userInputCopy, userInput);

	token = strtok_r(userInputCopy, " ", &savePtr);
	if (!token || (strcmp(token, "#") == 0)) {
		free(userInputCopy);
		free(command);
		return NULL;
	}

	initializeCommandStruct(command, numArgs);

	command->pathName = (char*)malloc((strlen(token) + strlen("/bin/") + 1) * sizeof(char));
	strcpy(command->pathName, "/bin/");
	strcat(command->pathName, token);

	command->argv[argvIndex] = command->pathName;
	argvIndex++;

	token = strtok_r(NULL, " ", &savePtr);
	while (token) {
		if (strcmp(token, "<") == 0) {
			command->inputRedirect = true;

			token = strtok_r(NULL, " ", &savePtr);
			command->newInput = (char*)malloc((strlen(token) + 1) * sizeof(char));

			strcpy(command->newInput, token);
		}
		else if (strcmp(token, ">") == 0) {
			command->outputRedirect = true;

			token = strtok_r(NULL, " ", &savePtr);
			command->newOutput = (char*)malloc((strlen(token) + 1) * sizeof(char));

			strcpy(command->newOutput, token);
		}
		else if (strcmp(token, "&") == 0) {
			command->backgroundProcess = true;
		}
		else {
			command->argv = appendArg(token, command->argv, numArgs, argvIndex);
			argvIndex++;
			numArgs++;
		}

		token = strtok_r(NULL, " ", &savePtr);
	}

	free(userInput);
	free(userInputCopy);

	return command;
}

void executeCommand(struct command* command) {
	int childStatus;
	pid_t spawnPid;

	spawnPid = fork();
	if (spawnPid == -1) {
		perror("fork");
		exit(1);
	}
	else if (spawnPid == 0) {
		execv(command->pathName, command->argv);
		perror("execv");
		exit(1);
	}
	else {
		spawnPid = waitpid(spawnPid, &childStatus, 0);
	}
}

void cleanupMemory(struct command* command) {
	int index = 0;

	while (command->argv[index]) {
		free(command->argv[index]);
		index++;
	}
	free(command->argv);

	if (command->newInput) {
		free(command->newInput);
	}

	if (command->newOutput) {
		free(command->newOutput);
	}

	free(command);
}

int main(void) {
	char* userInput = NULL;
	struct command* command = NULL;

	while (true) {
		userInput = getCommandLineInput();
		// remove this later
		if (strcmp(userInput, "exit") == 0) {
			break;
		}

		command = parseUserInput(userInput);

		if (!command) {
			free(userInput);
			continue;
		}

		executeCommand(command);

		cleanupMemory(command);
	}

	free(userInput);

	return EXIT_SUCCESS;
}
