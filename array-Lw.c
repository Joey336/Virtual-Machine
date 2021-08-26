#include <stdlib.h>
#include <string.h>

#include "array-Lw.h"
#include "arrayrep.h"
#include "mem.h"
#define T Array_T
T Array_new(int length, int size) {
	T array;
	NEW(array);
	if (length > 0)
		ArrayRep_init(array, length, size,
			CALLOC(length, size));
	else
		ArrayRep_init(array, length, size, NULL);
	return array;
}
void ArrayRep_init(T array, int length, int size,
	void *ary) {
	array->length = length;
	array->size   = size;
	if (length > 0)
		array->array = ary;
	else
		array->array = NULL;
}
void Array_free(T *array) {
	FREE((*array)->array);
	FREE(*array);
}
void *Array_get(T array, int i) {
	return array->array + i*array->size;
}
void *Array_put(T array, int i, void *elem) {
	memcpy(array->array + i*array->size, elem,
		array->size);
	return elem;
}
int Array_length(T array) {
	return array->length;
}
int Array_size(T array) {
	return array->size;
}
void Array_resize(T array, int length) {
	if (length == 0)
		FREE(array->array);
	else if (array->length == 0)
		array->array = ALLOC(length*array->size);
	else
		RESIZE(array->array, length*array->size);
	array->length = length;
}
T Array_copy(T array, int length) {
	T copy;
	copy = Array_new(length, array->size);
	if (copy->length >= array->length
	&& array->length > 0)
		memcpy(copy->array, array->array,
			array->length*array->size);
	else if (array->length > copy->length
	&& copy->length > 0)
		memcpy(copy->array, array->array,
			copy->length*array->size);
	return copy;
}
