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
};

struct command* getCommandLineInput(void) {
	char* userInput = NULL;
	size_t length;

	printf(": ");
	getline(&userInput, &length, stdin);
	*(userInput + strlen(userInput) - 1) = '\0';

	printf("%s\n", userInput);
}

int main(void) {
	struct command* userInput = NULL;

	while (true) {
		userInput = getCommandLineInput();
	}

	return EXIT_SUCCESS;
}
