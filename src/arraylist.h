#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "utility.h"

#define VECTOR_INIT_CAPACITY 4

#define VECTOR_INIT(vec) \
    Vector vec;          \
    vectorInit(&vec)
#define VECTOR_ADD(vec, item) vectorAdd(&vec, (void *)item)
#define VECTOR_SET(vec, i, item) vectorSet(&vec, i, (void *)item)
#define VECTOR_GET(vec, type, i) (type) vectorGet(&vec, i)
#define VECTOR_DELETE(vec, i) vectorDelete(&vec, i)
#define VECTOR_TOTAL(vec) vectorTotal(&vec)
#define VECTOR_FREE(vec) vectorFree(&vec)

typedef struct {
    void **items;
    i32 capacity;
    i32 total;
} Vector;

void vectorInit(Vector *vec);
void vectorAdd(Vector *vec, void *item);
void vectorSet(Vector *vec, i32 i, void *item);
void *vectorGet(Vector *vec, i32 i);
void vectorDelete(Vector *vec, i32 i);
i32 vectorTotal(Vector *vec);
void vectorFree(Vector *vec);
void vectorResize(Vector *vec, i32 size);

#endif
