/*
* Author: Colin Francis
* ONID: francico
* Description: Header file for dynamic array implementation
*/

struct dynamicArray {
	int size;
	int capacity;
	int* staticArray;
};

void upsizeArray(struct dynamicArray* dynArr);

void delete(struct dynamicArray* dynArr, int indexToDelete);

void append(struct dynamicArray* dynArr, int value);

struct dynamicArray* newDynamicArray(void);
