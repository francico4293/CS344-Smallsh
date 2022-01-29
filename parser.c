/*
* Author: Colin Francis
* ONID: francico
* Description: Functions used for command line parsing
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

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
	fflush(stdout);

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

char* populateCurrExpandedArg(char* currExpandedArg, char* startToken, char* endToken) {
	int index = 0;

	while (*startToken && startToken < endToken) {
		currExpandedArg[index] = *startToken;
		startToken++;
		index++;
	}
	currExpandedArg[index] = '\0';

	return startToken;
}

char* updateFinalExpandedArg(char* finalExpandedArg, char* currExpandedArg) {
	char* temp = NULL;

	if (!finalExpandedArg) {
		finalExpandedArg = (char*)malloc((strlen(currExpandedArg) + 1) * sizeof(char));
		strcpy(finalExpandedArg, currExpandedArg);

		free(currExpandedArg);
		currExpandedArg = NULL;
	}
	else {
		temp = finalExpandedArg;
		finalExpandedArg = (char*)malloc((strlen(finalExpandedArg) + strlen(currExpandedArg) + 1) * sizeof(char));
		strcpy(finalExpandedArg, temp);
		strcat(finalExpandedArg, currExpandedArg);

		free(currExpandedArg);
		currExpandedArg = NULL;

		free(temp);
		temp = NULL;
	}

	return finalExpandedArg;
}

char* parseArg(char* arg) {
	int index;
	char* startToken = arg;
	char* endToken = NULL;
	char* finalExpandedArg = NULL;
	char* currExpandedArg = NULL;
	char* temp = NULL;
	int pidLength = snprintf(NULL, 0, "%d", getpid());
	char* pidString = (char*)malloc((pidLength + 1) * sizeof(char));

	sprintf(pidString, "%d", getpid());

	endToken = strstr(startToken, "$$");
	while (endToken) {
		currExpandedArg = (char*)malloc((pidLength + (endToken - startToken) + 1) * sizeof(char));

		if (startToken == endToken) {
			strcpy(currExpandedArg, pidString);
		}
		else {
			startToken = populateCurrExpandedArg(currExpandedArg, startToken, endToken);
			strcat(currExpandedArg, pidString);
		}

		finalExpandedArg = updateFinalExpandedArg(finalExpandedArg, currExpandedArg);

		startToken = startToken + 2;
		endToken = strstr(startToken, "$$");
	}

	if (strlen(startToken) > 0) {
		currExpandedArg = (char*)malloc((strlen(startToken) + 1) * sizeof(char));

		startToken = populateCurrExpandedArg(currExpandedArg, startToken, arg + strlen(arg));

		finalExpandedArg = updateFinalExpandedArg(finalExpandedArg, currExpandedArg);
	}

	free(pidString);
	pidString = NULL;

	return finalExpandedArg;
}

char** appendArg(char* arg, char* argv[], int numArgs, int argvIndex) {
	int index = 0;
	char** newArgv = (char**)malloc((numArgs + 1) * sizeof(char*));

	arg = parseArg(arg);

	argv[argvIndex] = (char*)malloc((strlen(arg) + 1) * sizeof(char));
	strcpy(argv[argvIndex], arg);

	while (index < numArgs) {
		newArgv[index] = argv[index];
		index++;
	}
	newArgv[index] = NULL;

	free(arg);
	arg = NULL;

	free(argv);
	argv = NULL;

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
	if (!token || (*token == '#')) {
		free(userInputCopy);
		free(command);
		return NULL;
	}

	initializeCommandStruct(command, numArgs);

	command->pathName = (char*)malloc((strlen(token) + 1) * sizeof(char));
	strcpy(command->pathName, token);

	command->argv[argvIndex] = command->pathName;
	argvIndex++;

	token = strtok_r(NULL, " ", &savePtr);
	while (token) {
		if (strcmp(token, "<") == 0) {
			command->inputRedirect = true;

			token = strtok_r(NULL, " ", &savePtr);
			token = parseArg(token);
			command->newInput = (char*)malloc((strlen(token) + 1) * sizeof(char));

			strcpy(command->newInput, token);

			free(token);
		}
		else if (strcmp(token, ">") == 0) {
			command->outputRedirect = true;

			token = strtok_r(NULL, " ", &savePtr);
			token = parseArg(token);
			command->newOutput = (char*)malloc((strlen(token) + 1) * sizeof(char));

			strcpy(command->newOutput, token);

			free(token);
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
