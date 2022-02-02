/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Desciption: Header file for functions associated with memory cleanup
*/

/*
* Releases all memory allocated for the command struct and for use with the attributes of
* the command struct
*/
void cleanupMemory(struct command* command);

/*
* Releases all memory allocated for the command struct and for use with the the attributes of
* the command struct. Terminates any open background processes. Releases memory allocated for
* the dynamic array used to track runnning background processes
*/
void cleanupMemoryAndExit(struct command* command, struct dynamicArray* backgroundPids);
