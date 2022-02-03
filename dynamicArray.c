/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: An implementation of a dynamic array data structure
*/
#include <stdlib.h>

/*
* A struct representing a dynamic array
* Reference citations B, C, D
*/
struct dynamicArray {
	int size;  // the number of elements currently in the dynamic array
	int capacity;  // the number of element the dynamic array can hold
	int* staticArray;  // the underlying static array
};

/*
* Increases the capacity of the dynamic array by 2x its current size. A new underlying static array
* with this increased capacity is created and all elements in the current static array are transferred
* into the new static array
* Reference citations B, C, D
*/
void upsizeArray(struct dynamicArray* dynArr) {
	// allocate memory for a new static array whose capacity is 2x the current capacity
	int* newStaticArray = (int*)calloc((dynArr->capacity * 2), sizeof(int));

	// transfer all elements in the current static array into the new static array
	for (int index = 0; index < dynArr->size; index++) {
		newStaticArray[index] = dynArr->staticArray[index];
	}

	// update the capacity attribute of the dynamic array struct to 2x its current value
	dynArr->capacity = 2 * dynArr->capacity;

	// free memory allocated for the current static array
	free(dynArr->staticArray);
	// set the new static array as the new underlying static array for the dynamic array struct
	dynArr->staticArray = newStaticArray;
}

/*
* Deletes the element at the specified index from the dynamic array
* Reference citations B, C, D
*/
void delete(struct dynamicArray* dynArr, int indexToDelete) {
	// declare and initialize a variable to track the current index position to add values at in the
	// new static array
	int newIndex = 0;
	// allocate memory for a new static array - all elements except the element at the index to delete
	// will be transferred into this new static array
	int* newStaticArray = (int*)calloc((dynArr->capacity * 2), sizeof(int));

	// iterate over each element in the current dynamic array
	for (int index = 0; index < dynArr->size; index++) {
		// skip the element who is positioned at the index position we are deleting
		if (index == indexToDelete) {
			continue;
		}

		// add element to new static array at position newIndex
		newStaticArray[newIndex] = dynArr->staticArray[index];
		// increment newIndex by 1
		newIndex++;
	}

	// free memory allocated for the current static array
	free(dynArr->staticArray);
	// set the newStaticArray as the underlying static array for the dynamic array struct
	dynArr->staticArray = newStaticArray;

	// decrease the size of the dynamic array by one since we removed one element
	dynArr->size--;
}

/*
* Add the specified value to the end of the dynamic array
* Reference citations B, C, D
*/
void append(struct dynamicArray* dynArr, int value) {
	// if the ratio of elements in the array to array capacity is greater than or equal to 0.75, then
	// upsize the underlying static array
	if (((float)(dynArr->size + 1) / (float)(dynArr->capacity)) >= 0.75) {
		upsizeArray(dynArr);
	}

	// add the specified value to the end of the dynamic array
	dynArr->staticArray[dynArr->size] = value;
	// increase the size of the dynamic array by one since a new element has been added
	dynArr->size++;
}

/*
* Creates a new instance of a dynamic array
* Reference citations B, C, D
*/
struct dynamicArray* newDynamicArray(void) {
	// allocate memory for a new static array struct
	struct dynamicArray* dynArr = (struct dynamicArray*)calloc(1, sizeof(struct dynamicArray));

	// initialize the size to 0
	dynArr->size = 0;
	// initialize the capacity to 5
	dynArr->capacity = 5;
	// allocate memory large enough to hold the number of integers specified by the capacity attribute
	dynArr->staticArray = (int*)malloc(dynArr->capacity * sizeof(int));

	// return a pointer to the dynamic array struct
	return dynArr;
}
