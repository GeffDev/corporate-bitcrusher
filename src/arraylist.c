#include "arraylist.h"

void vectorInit(Vector *vec) {
    vec->capacity = VECTOR_INIT_CAPACITY;
    vec->total = 0;
    vec->items = calloc(vec->capacity, sizeof(void *));
}

void vectorAdd(Vector *vec, void *item) {
    if (vec->total == vec->capacity) {
        vectorResize(vec, vec->capacity * 2);
        vec->items[vec->total++] = item;
    }
}

void vectorSet(Vector *vec, i32 i, void *item) {
    if (i >= 0 && i < vec->total) {
        vec->items[i] = item;
    }
}

void *vectorGet(Vector *vec, i32 i) {
    if (i >= 0 && i < vec->total) {
        return vec->items[i];
    }

    return NULL;
}

void vectorDelete(Vector *vec, i32 i) {
    if (i < 0 || i >= vec->total) {
        return;
    }

    vec->items[i] = NULL;

    for (i32 index = i; index < vec->total - 1; index++) {
        vec->items[index] = vec->items[index + 1];
        vec->items[index + 1] = NULL;
    }

    --vec->total;

    if (vec->total > 0 && vec->total == vec->capacity / 4) {
        vectorResize(vec, vec->capacity / 2);
    }
}

i32 vectorTotal(Vector *vec) {
    return vec->total;
}

void vectorFree(Vector *vec) {
    free(vec->items);
}

void vectorResize(Vector *vec, i32 size) {
    void **items = realloc(vec->items, sizeof(void *) * size);

    if (items) {
        vec->items = items;
        vec->capacity = size;
    }
}