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

void upsizeArray(struct dynamicArray* dynArr) {
	int* newStaticArray = (int*)calloc((dynArr->capacity * 2), sizeof(int));

	for (int index = 0; index < dynArr->size; index++) {
		newStaticArray[index] = dynArr->staticArray[index];
	}

	dynArr->capacity = 2 * dynArr->capacity;

	free(dynArr->staticArray);
	dynArr->staticArray = newStaticArray;
}

void delete(struct dynamicArray* dynArr, int indexToDelete) {
	int newIndex = 0;
	int* newStaticArray = (int*)calloc((dynArr->capacity * 2), sizeof(int));

	for (int index = 0; index < dynArr->size; index++) {
		if (index == indexToDelete) {
			continue;
		}

		newStaticArray[newIndex] = dynArr->staticArray[index];
		newIndex++;
	}

	free(dynArr->staticArray);
	dynArr->staticArray = newStaticArray;

	dynArr->size--;
}

void append(struct dynamicArray* dynArr, int value) {
	if (((float)(dynArr->size + 1) / (float)(dynArr->capacity)) >= 0.75) {
		upsizeArray(dynArr);
	}

	dynArr->staticArray[dynArr->size] = value;
	dynArr->size++;
}

struct dynamicArray* newDynamicArray(void) {
	struct dynamicArray* dynArr = (struct dynamicArray*)calloc(1, sizeof(struct dynamicArray));

	dynArr->size = 0;
	dynArr->capacity = 5;
	dynArr->staticArray = (int*)malloc(dynArr->capacity * sizeof(int));

	return dynArr;
}
