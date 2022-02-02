/*
* Author: Colin Francis
* ONID: francico
* Title: Smallsh
* Description: Header file for dynamic array implementation
*/

/*
* A struct representing a dynamic array
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
*/
void upsizeArray(struct dynamicArray* dynArr);

/*
* Deletes the element at the specified index from the dynamic array
*/
void delete(struct dynamicArray* dynArr, int indexToDelete);

/*
* Add the specified value to the end of the dynamic array
*/
void append(struct dynamicArray* dynArr, int value);

/*
* Creates a new instance of a dynamic array
*/
struct dynamicArray* newDynamicArray(void);
