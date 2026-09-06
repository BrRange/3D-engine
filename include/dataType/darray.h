#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include "rustydef.h"

extern void *(*darrayRealloc)(void *mem, usz bytes);

typedef struct DArray{
  void *data;
  u32 len, cap, type;
} DArray;

DArray darray_new(u32 type);

void *darray_get(const DArray darray, u32 index);

void darray_grow(DArray *darray, u32 target);

void darray_shrink(DArray *darray);

void darray_append(DArray *darray, void *element);

void darray_appendPtr(DArray *darray, void *element);

void darray_appendMany(DArray *darray, void *elementList, u32 elementCount);

void darray_remove(DArray *darray, u32 index);

void darray_removeMany(DArray *darray, u32 index, u32 amount);

void darray_pop(DArray *darray, u32 index);

void darray_popMany(DArray *darray, u32 index, u32 amount);

void darray_destroy(DArray *darray);

#endif