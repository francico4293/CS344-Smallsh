/*
* Author: Colin Francis
* ONID: francico
* Desciption: Header file for functions associated with memory cleanup
*/

void cleanupMemory(struct command* command);

void cleanupMemoryAndExit(struct command* command, struct dynamicArray* backgroundPids);
