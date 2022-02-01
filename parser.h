/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Header file for parser functions
*/

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
char* getCommandLineInput(void);

/*
* Used to initialize the command struct which is used to maintain the details of the command
* provided by the user at the command line.
*/
void initializeCommandStruct(struct command* command, int numArgs);

/*
* Populates the memory segment starting at the address in currExpandedArg with all characters
* found starting at the address in startToken and up to, but not including, the address in endToken
*/
char* populateCurrExpandedArg(char* currExpandedArg, char* startToken, char* endToken);

/*
* Effectively concatenates the characters in currExpandedArg to the characters in the memory segment
* maintained by finalExpandedArg
*/
char* updateFinalExpandedArg(char* finalExpandedArg, char* currExpandedArg);

/*
* Parses each individual argument being added to the argv member of the command struct and
* searches for any instances of "$$". If "$$" is found, it is expanded into the process ID
* of smallsh itself.
*/
char* parseArg(char* arg);

/*
* Appends the current arg to the argv array member of the command struct.
*/
char** appendArg(char* arg, char* argv[], int numArgs, int argvIndex);

/*
* Fully parses the userInput string and sets / updates the appropriate members of the command struct
* instance that is built for use in executing the user provided command
*/
struct command* parseUserInput(char* userInput);
