#include "heap.h"

void build_min_heap(heap_t *h) {
    for (int i = h->tail / 2; i >= 1; i--)
        min_heapify(h, i);
}

void min_heapify(heap_t *h, int curr) {
    int l_child = (curr * 2 <= h->tail) ? curr * 2 : -1;
    int r_child = ((curr << 1) + 1 <= h->tail) ? (curr << 1) + 1 : -1;

    int min = curr;
    if (l_child != -1 && h->node[l_child].value < h->node[min].value)
        min = l_child;
    if (r_child != -1 && h->node[r_child].value < h->node[min].value)
        min = r_child;

    if (min != curr) {
        swap(&h->node[curr], &h->node[min]);
        min_heapify(h, min);
    }
}

int get_min_value(heap_t *h) { return h->node[1].value; }

int get_min_index(heap_t *h) { return h->node[1].index; }

void swap(node_t *n1, node_t *n2) {
    /* Swaps the values */
    n1->value ^= n2->value;
    n2->value ^= n1->value;
    n1->value ^= n2->value;
    /* Swaps the indices */
    n1->index ^= n2->index;
    n2->index ^= n1->index;
    n1->index ^= n2->index;
}
