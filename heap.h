#ifndef HEAP_H
#define HEAP_H

typedef struct node {
    int value;
    int index;
} node_t;

typedef struct heap {
    node_t *node; /* Note: index 0 will not be used */
    int tail;     /* tail is the index of the last node in the heap */
} heap_t;

void build_min_heap(heap_t *h);
void min_heapify(heap_t *h, int curr);
int get_min_value(heap_t *h);
int get_min_index(heap_t *h);
void swap(node_t *n1, node_t *n2);

#endif