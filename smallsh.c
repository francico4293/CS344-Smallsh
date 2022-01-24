/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct command {
	char* pathName;
	char** argv;
	bool inputRedirect;
	char* newInput;
	bool outputRedirect;
	char* newOutput;
};

char* getCommandLineInput(void) {
	char* userInput = NULL;
	char* inputBuffer = NULL;
	size_t length;

	printf(": ");
	getline(&inputBuffer, &length, stdin);
	*(inputBuffer + strlen(inputBuffer) - 1) = '\0';

	userInput = (char*)malloc((strlen(inputBuffer) + 1) * sizeof(char));
	strcpy(userInput, inputBuffer);

	return userInput;
}

struct command* parseUserInput(char* userInput) {
	char* token = NULL;
	char* savePtr = NULL;
	struct command* command = (struct command*)malloc(sizeof(struct command));

	token = strtok_r(userInput, " ", &savePtr);
	if (!token || (strcmp(token, "#") == 0)) {
		return NULL;
	}
	command->pathName = (char*)malloc((strlen(token) + strlen("/bin/") + 1) * sizeof(char));
	strcpy(command->pathName, "/bin/");
	strcat(command->pathName, token);

	return command;
}

int main(void) {
	char* userInput = NULL;
	struct command* command = NULL;

	while (true) {
		userInput = getCommandLineInput();
		command = parseUserInput(userInput);
		if (!command) {
			continue;
		}
	}

	return EXIT_SUCCESS;
}
