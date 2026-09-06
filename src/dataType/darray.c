#include "datatype/darray.h"
#include <stdlib.h>
#include <string.h>

void *(*darrayRealloc)(void *mem, usz bytes) = realloc;

DArray darray_new(u32 type){
  DArray darray = {.type = type};
  return darray;
}

void *darray_get(const DArray darray, u32 index){
  return darray.data + ((usz)index * darray.type);
}

void darray_grow(DArray *darray, u32 target){
  u32 cap = darray->cap;
  while(cap < target){
    u32 incr = (cap + 1) >> 1;
    cap = (cap + incr + 2) & -2u;
  }
  if(cap > darray->cap){
    darray->data = darrayRealloc(darray->data, cap * darray->type);
    darray->cap = cap;
  }
}

void darray_shrink(DArray *darray){
  u32 cap = darray->cap;
  while(cap / 2 > darray->len) cap /= 2;
  if(cap < darray->cap){
    darray->data = darrayRealloc(darray->data, cap * darray->type);
    darray->cap = cap;
  }
}

void darray_append(DArray *darray, void *element){
  darray_grow(darray, darray->len + 1);
  u8 (*sized)[darray->type] = darray->data;
  memcpy(sized + darray->len, element, darray->type);
  ++darray->len;
}

void darray_appendPtr(DArray *darray, void *element){
  darray_append(darray, &element);
}

void darray_appendMany(DArray *darray, void *elementList, u32 elementCount){
  darray->len += elementCount;
  darray_grow(darray, darray->len);
  u8 (*sized)[darray->type] = darray->data;
  memcpy(sized + darray->len - elementCount, elementList, elementCount * darray->type);
}

void darray_remove(DArray *darray, u32 index){
  u8 (*sized)[darray->type] = darray->data;
  --darray->len;
  if(index < darray->len)
  memmove(sized + index, sized + index + 1, (darray->len - index) * darray->type);
}

void darray_removeMany(DArray *darray, u32 index, u32 amount){
  u8 (*sized)[darray->type] = darray->data;
  darray->len -= amount;
  if(index < darray->len)
  memmove(sized + index, sized + index + amount, (darray->len - index) * darray->type);
}

void darray_pop(DArray *darray, u32 index){
  u8 (*sized)[darray->type] = darray->data;
  --darray->len;
  if(index < darray->len)
  memcpy(sized + index, sized + darray->len, darray->type);
}

void darray_popMany(DArray *darray, u32 index, u32 amount){
  u8 (*sized)[darray->type] = darray->data;
  darray->len -= amount;
  u32 diff = amount + index > darray->len ? amount + index - darray->len : 0;
  if(index < darray->len)
  memcpy(sized + index, sized + darray->len + diff, (amount - diff) * darray->type);
}

void darray_destroy(DArray *darray){
  free(darray->data);
  memset(darray, 0, sizeof(*darray));
}