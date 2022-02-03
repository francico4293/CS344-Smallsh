/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Functions used for command line parsing
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

/*
* A struct used in command line parsing. This struct holds all the details about the 
* type of command that was entered by the user
*/
struct command {
	char* pathName;  // the pathname of the binary executable
	char** argv;  // an array of arguments
	bool inputRedirect;  // true if input should be redirected, otherwise false
	char* newInput;  // the file to redirect input from
	bool outputRedirect;  // true if output should be redirected, otherwise false
	char* newOutput;  // the file to redirect output to
	bool backgroundProcess;  // true if the process should run in the background, otherwise false
};

/*
* Displays a colon ":" symbol as a prompt for each command line. Captures any input provided by
* the user and returns that input as a character pointer.
*/
char* getCommandLineInput(void) {
	// declare and initialize a character pointer used to store user command line input
	char* userInput = NULL;
	// for use with getline
	size_t length;
	// for use with getline
	ssize_t nread;

	// display ":" as command line prompt
	printf(": ");
	// use getline to capture user input from stdin and store in userInput variable
	nread = getline(&userInput, &length, stdin);
	// remove '\n' resulting from the user pressing enter and replace with null character
	*(userInput + strlen(userInput) - 1) = '\0';
	// flush standard output
	fflush(stdout);

	// return user input as character pointer
	return userInput;
}

/*
* Used to initialize the command struct which is used to maintain the details of the command
* provided by the user at the command line.
*/
void initializeCommandStruct(struct command* command, int numArgs) {
	// initialize input redirection as false
	command->inputRedirect = false;
	// initialize new input source as NULL
	command->newInput = NULL;

	// initialize output redirection as false
	command->outputRedirect = false;
	// intialize new output source as NULL
	command->newOutput = NULL;

	// initialize background process as false
	command->backgroundProcess = false;

	// allocate memory large enough to store numArgs number of character pointers
	command->argv = (char**)malloc(numArgs * sizeof(char*));
	// initialize each character pointer in the argv array to be NULL
	for (int index = 0; index < numArgs; index++) {
		command->argv[index] = NULL;
	}
}

/*
* Populates the memory segment starting at the address in currExpandedArg with all characters 
* found starting at the address in startToken and up to, but not including, the address in endToken
*/
char* populateCurrExpandedArg(char* currExpandedArg, char* startToken, char* endToken) {
	// declare and initialize a variable to maintain index position relative to currExpandedArg
	int index = 0;

	// iterate while startToken is not NULL and while the address in startToken is less than the
	// address in endToken
	while (*startToken && startToken < endToken) {
		// set the address of currExpandedArg + index equal to the character found at the current
		// address in startToken
		currExpandedArg[index] = *startToken;
		// increment the address in startToken up by one byte
		startToken++;
		// increment the value in index by one
		index++;
	}
	// null terminate the segment of memory that the address in currExpandedArg holds
	currExpandedArg[index] = '\0';

	// return the address that startToken finished at after adding characters to currExpandedArg
	return startToken;
}

/*
* Effectively concatenates the characters in currExpandedArg to the characters in the memory segment
* maintained by finalExpandedArg
*/
char* updateFinalExpandedArg(char* finalExpandedArg, char* currExpandedArg) {
	// declare and initialize a variable used to temporarily maintain a memory address to avoid losing
	// access to it
	char* temp = NULL;

	// if finalExpandedArg is a NULL pointer, then nothing has been added to it yet
	if (!finalExpandedArg) {
		// allocate memory large enough to hold all of the characters in the memory segment maintained
		// by currExpandedArg plus one additional byte for the null character
		finalExpandedArg = (char*)malloc((strlen(currExpandedArg) + 1) * sizeof(char));
		// copy all characters from currExpandedArg's memory segment into the memory segment being pointed
		// to by finalExpandedArg
		strcpy(finalExpandedArg, currExpandedArg);

		// release the memory allocated by currExpandedArg and set it to NULL
		free(currExpandedArg);
		currExpandedArg = NULL;
	}
	// if finalExpandedArg is not NULL, then it is maintaining what will be the full final expanded arg - we must
	// add on to it
	else {
		// allow temp to hold the memory address in finalExpandedArg while we increase the segment of memory it will
		// represent
		temp = finalExpandedArg;
		// allocate a segment of memory large enough to hold the current length of finalExpandedArg plus the length
		// of currExpandedArg plus an additional byte for the null character
		finalExpandedArg = (char*)malloc((strlen(finalExpandedArg) + strlen(currExpandedArg) + 1) * sizeof(char));
		// transfer characters originally being pointed to by finalExpandedArg back to the larger memory segment just
		// allocated
		strcpy(finalExpandedArg, temp);
		// concatenate finalExpandedArg with currExpandedArg to fully update finalExpandedArg
		strcat(finalExpandedArg, currExpandedArg);

		// release the memory allocated by currExpandedArg and set it to NULL
		free(currExpandedArg);
		currExpandedArg = NULL;

		// release the memory segement temp points to and set temp to NULL
		free(temp);
		temp = NULL;
	}

	// return updated finalExpandedArg as a character pointer
	return finalExpandedArg;
}

/*
* Parses each individual argument being added to the argv member of the command struct and
* searches for any instances of "$$". If "$$" is found, it is expanded into the process ID
* of smallsh itself.
*/
char* parseArg(char* arg) {
	int index;
	// delcare and initialize a character pointer that will consistently be pointing to the first character
	// in the provided arg or the first character following an instance of "$$"
	char* startToken = arg;
	// declare and initialize a character pointer that will consistently be pointing to the next instance
	// of "$$" if one is found
	char* endToken = NULL;
	// declare and initialize a variable that will maintain the final epxanded argument state
	char* finalExpandedArg = NULL;
	// declare and initialize a variable that will maintain the current expanded argument state
	char* currExpandedArg = NULL;
	// declare and initialize a variable that stores the number of digits in the pid of smallsh (reference citation E)
	int pidLength = snprintf(NULL, 0, "%d", getpid());
	// allocate memory large enough to hold characters equal to the length of the pid of smallsh
	// plus a null character
	char* pidString = (char*)malloc((pidLength + 1) * sizeof(char));

	// populate pidString with characters representing the current pid of smallsh
	sprintf(pidString, "%d", getpid());

	// search for the first instance of "$$" starting from startToken's address
	endToken = strstr(startToken, "$$");
	// iterate until no more instances of "$$" are detected
	while (endToken) {
		// since endToken is the first address of the first instance of "$$" and startToken is the address
		// of the first character in the provided arg, the following memory allocation will allocate memory
		// large enough to hold all characters found starting from the address in startToken and up to but 
		// not including the address in endToken plus the length of the pid of smallsh plus an additional
		// byte to store the null character
		currExpandedArg = (char*)malloc((pidLength + (endToken - startToken) + 1) * sizeof(char));

		// if the address in startToken is equal to the address in endToken, then both addresses are pointing
		// to an instance of "$$"
		if (startToken == endToken) {
			// copy the contents of pidString into currExpandedArg - currExpandedArg will only hold the pid
			// of smallsh
			strcpy(currExpandedArg, pidString);
		}
		// if the address in startToken is different than the address in endToken, then startToken is pointing
		// to a character and endToken is pointing to an instance of "$$"
		else {
			// populate currExpandedArg with the characters found starting at startToken and up to, but not
			// including endToken
			startToken = populateCurrExpandedArg(currExpandedArg, startToken, endToken);
			// concatenate currExpandedArg and the pid of smallsh to account for the expansion of "$$"
			strcat(currExpandedArg, pidString);
		}

		// add the characters of currExpandedArg to the array of characters being maintained by finalExpandedArg
		finalExpandedArg = updateFinalExpandedArg(finalExpandedArg, currExpandedArg);

		// position startToken two bytes up from it's current position since after processing it will be pointing
		// at endToken which is pointing at "$$". Moving startToken up 2 bytes allows it to point to the first
		// character following "$$"
		startToken = startToken + 2;
		// find the next instance of "$$" using startToken as the starting address
		endToken = strstr(startToken, "$$");
	}

	// after exiting the while loop check if there are any characters remaining since the while loop only breaks
	// when no more instances of "$$" are found - there may be additional characters following the last instance
	// of "$$"
	if (strlen(startToken) > 0) {
		// allocate memory large enough to hold all remaining characters plus the null character
		currExpandedArg = (char*)malloc((strlen(startToken) + 1) * sizeof(char));

		// populate currExpandedArg with the remaining characters - endToken will be the address of the very last
		// character in arg, i.e. arg + strlen(arg)
		startToken = populateCurrExpandedArg(currExpandedArg, startToken, arg + strlen(arg));

		// add the characters of currExpandedArg to the array of characters being maintained by finalExpandedArg
		finalExpandedArg = updateFinalExpandedArg(finalExpandedArg, currExpandedArg);
	}

	// free memory allocated by pidString and set pidString to NULL
	free(pidString);
	pidString = NULL;

	// return the final fully expanded arg as a character pointer
	return finalExpandedArg;
}

/*
* Appends the current arg to the argv array member of the command struct.
*/
char** appendArg(char* arg, char* argv[], int numArgs, int argvIndex) {
	// declare and initialize a variable to maintain index position relative to newArgv array
	int index = 0;
	// allocate memory large enough to hold the current number of character pointers in argv plus
	// one to hold the new arg being appended
	char** newArgv = (char**)malloc((numArgs + 1) * sizeof(char*));

	// parse the current arg being appended and expand each instance of "$$" found
	arg = parseArg(arg);

	// argv index will always be NULL terminated and argvIndex will always be holding the index
	// position of the next argument. At this point, *(argv + argvIndex) will always exist and
	// will always be NULL. Before transferring every character pointer in argv to newArgv, 
	// allocate memory large enough to hold arg after it has been parsed and copy the characters
	// in arg to the address argv + argvIndex
	argv[argvIndex] = (char*)malloc((strlen(arg) + 1) * sizeof(char));
	strcpy(argv[argvIndex], arg);

	// copy every character pointer in argv into newArgv
	while (index < numArgs) {
		newArgv[index] = argv[index];
		index++;
	}
	// set the very last index position of newArgv to NULL as is expected by execvp
	newArgv[index] = NULL;

	// free memory allocated by arg and set arg to NULL
	free(arg);
	arg = NULL;

	// free memory allocated by argv and set srgv to NULL
	free(argv);
	argv = NULL;

	// return new argv as a pointer to a character pointer
	return newArgv;
}

/*
* Fully parses the userInput string and sets / updates the appropriate members of the command struct
* instance that is built for use in executing the user provided command
*/
struct command* parseUserInput(char* userInput) {
	// declare and initialize a variable used to maintain the numbers of elements in the argv array
	// of the command struct initialized below. Num args includes NULL as the last element in the argv
	// array will always be NULL
	int numArgs = 2;
	// declare and initialize a variable used to maintain the index position to insert the next arg at
	// in the argv array
	int argvIndex = 0;
	// declare and initialize a variable used to maintain the last token seen during parsing. This will be
	// used to verify if the lastToken is "&" which will signal that the command should be run as a
	// background process
	char* lastToken = NULL;
	// declare and initialize a variable used to maintain the index of where the lastToken exists in the argv
	// array - this allows setting this element to NULL if lastToken is "&"
	int lastTokenIndex = 0;
	// declare and initialize a variable to maintain each token while parsing userInput
	char* token = NULL;
	// declare and initialize a variable to maintain the position in userInput while parsing
	char* savePtr = NULL;
	// allocate memory large enough to hold userInput plus an additional byte for the null character
	char* userInputCopy = (char*)malloc((strlen(userInput) + 1) * sizeof(char));
	// allocate memory large enough to hold the command struct
	struct command* command = (struct command*)malloc(sizeof(struct command));

	// parsing of userInput will effect userInput. In order to free this memory later, the original
	// userInput address is preserved by copying the characters into userInputCopy for parsing
	strcpy(userInputCopy, userInput);

	// get the first token
	token = strtok_r(userInputCopy, " ", &savePtr);
	// if the first token is NULL, then the input was empty, if the first character in the first token
	// is '#', then the user entered a comment - in either case we will just return the user back to the
	// command prompt
	if (!token || (*token == '#')) {
		// free memory allocated by userInputCopy
		free(userInputCopy);
		// free memory allocated by command
		free(command);
		// return a NULL pointer
		return NULL;
	}

	// initialize the command struct which will hold components of the parsed userInput
	initializeCommandStruct(command, numArgs);

	// the first token will be the actual command provided by the user and will also be executed by using
	// the PATH variable if the command is not a built-in command - allocate memory for the pathName 
	// attribute large enough for the first token plus an additional byte for the null character
	command->pathName = (char*)malloc((strlen(token) + 1) * sizeof(char));
	// copy the characters of token into the memory segment allocated for pathName
	strcpy(command->pathName, token);

	// the first element of argv will also be the command provided by the user based on later usage of
	// execvp. Set argv[0] equal to the same memory segment that the pathName attribute is equal to
	command->argv[argvIndex] = command->pathName;
	// increment argvIndex by 1
	argvIndex++;

	// get the second token
	token = strtok_r(NULL, " ", &savePtr);
	// continue parsing userInput until everything has been parsed - when this happens, token will be NULL
	while (token) {
		// this if statement is being used to maintain the lastToken found as well as what index position
		// it was found at. After the while loop breaks a conditional check will be made against lastToken.
		// If lastToken is '&' then the command will need to be run in the background and the lastToken will
		// be replaced with NULL
		if (strcmp(token, " ") != 0) {
			// if lastToken is not NULL
			if (lastToken) {
				// free memory segment stored in lastToken before allocating any new memory
				free(lastToken);
			}

			// allocate memory large enough to store the current token plus an additional byte for the NULL
			// character
			lastToken = (char*)malloc((strlen(token) + 1) * sizeof(char));
			// update lastTokenIndex to be equal to the current value in argvIndex
			lastTokenIndex = argvIndex;
			// copy the characters in token into the memory segment maintained by lastToken
			strcpy(lastToken, token);
		}

		// if a '<' character is encountered then the user has specified input redirection
		if (strcmp(token, "<") == 0) {
			// set the inputRedirect attribute to true
			command->inputRedirect = true;

			// get the next token since the next token following '<' will be the location to redirect input from
			token = strtok_r(NULL, " ", &savePtr);
			// parse the current token to expand any instances of "$$"
			token = parseArg(token);
			// allocate memory large enough to hold the current token plus an additional byte for the NULL
			// character
			command->newInput = (char*)malloc((strlen(token) + 1) * sizeof(char));

			// copy the characters in token into the memory segment allocated for the newInput attribute
			strcpy(command->newInput, token);

			// free the memory allocated for token
			free(token);
		}
		else if (strcmp(token, ">") == 0) {
			// set the outputRedirect attribute to true
			command->outputRedirect = true;

			// get the next token since the next token following '>' will be the location to redirect output to
			token = strtok_r(NULL, " ", &savePtr);
			// parse the current token to expand any instances of "$$"
			token = parseArg(token);
			// allocate memory large enough to hold the current token plus an additional byte for the NULL
			// character
			command->newOutput = (char*)malloc((strlen(token) + 1) * sizeof(char));

			// copy the characters in token into the memory segment allocated for the newOutput attribute
			strcpy(command->newOutput, token);

			// free the memory allocated for token
			free(token);
		}
		else {
			// append the current token to the argv array attribute at the index position specified by argvIndex
			command->argv = appendArg(token, command->argv, numArgs, argvIndex);
			// increment argvIndex by one
			argvIndex++;
			// increment numArgs by one
			numArgs++;
		}

		// get the next token
		token = strtok_r(NULL, " ", &savePtr);
	}

	// if lastToken is equal to "&" then the command will need to be run as a background process
	if (lastToken && strcmp(lastToken, "&") == 0) {
		// set the backgroundProcess attribute of the command struct to true
		command->backgroundProcess = true;

		// free the memory segment represented by the address in argv[lastTokenIndex]
		free(command->argv[lastTokenIndex]);
		// set argv[lastTokenIndex] to NULL
		command->argv[lastTokenIndex] = NULL;

		// free the memory segment represented by the address in lastToken
		free(lastToken);
	}
	// if lastToken isn't equal to "&" we still need to free the memory segment
	else if (lastToken) {
		// free memory allocated for lastToken
		free(lastToken);
	}

	// free memory allocated for userInput
	free(userInput);
	// free memory allocated for userInputCopy
	free(userInputCopy);

	// return the address of the fully populated command struct
	return command;
}
