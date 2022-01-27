/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for parser functions
*/

struct command {
	char* pathName;
	char** argv;
	bool inputRedirect;  // <
	char* newInput;
	bool outputRedirect;  // >
	char* newOutput;
	bool backgroundProcess;  // &
};

char* getCommandLineInput(void);

void initializeCommandStruct(struct command* command, int numArgs);

char* populateCurrExpandedArg(char* currExpandedArg, char* startToken, char* endToken);

char* updateFinalExpandedArg(char* finalExpandedArg, char* currExpandedArg);

char* parseArg(char* arg);

char** appendArg(char* arg, char* argv[], int numArgs, int argvIndex);

struct command* parseUserInput(char* userInput);
