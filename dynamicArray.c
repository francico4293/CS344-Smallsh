/*
* Author: Colin Francis
* ONID: francico
* Description: An implementation of a dynamic array data structure
*/
#include <stdlib.h>

struct dynamicArray {
	int size;
	int capacity;
	int* staticArray;
};

void resizeArray(struct dynamicArray* dynArr) {

}

void append(struct dynamicArray* dynArr, int value) {
	if (((float)(dynamicArray->size + 1) / (float)(dynamicArray->capacity)) >= 0.75) {

	}

	dynamicArray->staticArray[dynamicArray->size] = value;
	dynamicArray->size++;
}

struct dynamicArray* newDynamicArray(void) {
	struct dynamicArray* dynArr = (struct dynamicArray*)malloc(sizeof(struct dynamicArray));

	dynArr->size = 0;
	dynArr->capacity = 5;
	dynArr->staticArray = (int*)malloc(dynArr->capacity * sizeof(int));

	return dynArr;
}
