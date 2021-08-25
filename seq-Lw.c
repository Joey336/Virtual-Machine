
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "seq-Lw.h"
#include "array-Lw.h"
#include "arrayrep.h"
#include "mem.h"
#define T Seq_T
struct T {
	struct Array_T array;
	int length;
	int head;
};
static void expand(T seq) {
	int n = seq->array.length;
	Array_resize(&seq->array, 2*n);
	if (seq->head > 0)
		{
			void **old = &((void **)seq->array.array)[seq->head];
			memcpy(old+n, old, (n - seq->head)*sizeof (void *));
			seq->head += n;
		}
}
T Seq_new(int hint) {
	T seq;
	NEW0(seq);
	if (hint == 0)
		hint = 16;
	ArrayRep_init(&seq->array, hint, sizeof (void *),
		ALLOC(hint*sizeof (void *)));
	return seq;
}
T Seq_seq(void *x, ...) {
	va_list ap;
	T seq = Seq_new(0);
	va_start(ap, x);
	for ( ; x; x = va_arg(ap, void *))
		Seq_addhi(seq, x);
	va_end(ap);
	return seq;
}
void Seq_free(T *seq) {
	Array_free((Array_T *)seq);
}
int Seq_length(T seq) {
	return seq->length;
}
void *Seq_get(T seq, int i) {
	return ((void **)seq->array.array)[
	       	(seq->head + i)%seq->array.length];
}
void *Seq_put(T seq, int i, void *x) {
	void *prev;
	prev = ((void **)seq->array.array)[
	       	(seq->head + i)%seq->array.length];
	((void **)seq->array.array)[
		(seq->head + i)%seq->array.length] = x;
	return prev;
}
void *Seq_remhi(T seq) {
	int i;
	i = --seq->length;
	return ((void **)seq->array.array)[
	       	(seq->head + i)%seq->array.length];
}
void *Seq_remlo(T seq) {
	int i = 0;
	void *x;
	x = ((void **)seq->array.array)[
	    	(seq->head + i)%seq->array.length];
	seq->head = (seq->head + 1)%seq->array.length;
	--seq->length;
	return x;
}
void *Seq_addhi(T seq, void *x) {
	int i;
	if (seq->length == seq->array.length)
		expand(seq);
	i = seq->length++;
	return ((void **)seq->array.array)[
	       	(seq->head + i)%seq->array.length] = x;
}
void *Seq_addlo(T seq, void *x) {
	int i = 0;
	if (seq->length == seq->array.length)
		expand(seq);
	if (--seq->head < 0)
		seq->head = seq->array.length - 1;
	seq->length++;
	return ((void **)seq->array.array)[
	       	(seq->head + i)%seq->array.length] = x;
}