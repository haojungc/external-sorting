#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_FILE_NAME_LENGTH 30
#define MAX_FILES 1000000
#define MAX_CHUNK_SIZE 10000

typedef struct node {
    int value;
    int index;
} node_t;

typedef struct heap {
    node_t *node; /* Note: index 0 will not be used */
    int tail;     /* tail is the index of the last node in the heap */
} heap_t;

/* Functions for heap_t */
static void build_min_heap(heap_t *h);
static void min_heapify(heap_t *h, int curr);
static int get_min_value(heap_t *h);
static int get_min_index(heap_t *h);
static void swap(node_t *n1, node_t *n2);

static FILE *open_file(const char *filename, const char *mode);
static void make_dir(const char *dir, mode_t mode);
static size_t split_file_and_sort(const char *);
static void external_merge_sort(const char *);
static void merge_sort(int[], int, int);
static void merge(int[], int, int, int);

FILE *fp[MAX_FILES];
int n[MAX_CHUNK_SIZE];

int main(int argc, char **argv) {
    /* Default file */
    char f_in[MAX_FILE_NAME_LENGTH] = "input2.txt";

    /* Too many arguments */
    if (argc > 2) {
        puts("Error: Too many arguments");
        puts("Usage: ./sort [input_file]");
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
        sscanf(argv[1], "%s", f_in);
    printf("Input file: %s\n", f_in);

    clock_t start, end;

    start = clock();

    external_merge_sort(f_in);

    end = clock();

    printf("Total Elapsed Time: %lf secs\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}

static FILE *open_file(const char *filename, const char *mode) {
    FILE *fp = fopen(filename, mode);
    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    return fp;
}

static void make_dir(const char *dir, mode_t mode) {
    /* Checks if dir exists */
    struct stat st;
    if (stat(dir, &st) == -1) {
        int mkdir_stat = mkdir(dir, mode);
        if (mkdir_stat == -1) {
            printf("Error: Cannot create directory %s", dir);
            exit(EXIT_FAILURE);
        }
    }
}

static size_t split_file_and_sort(const char *f_in) {
    clock_t start, end;
    start = clock();

    FILE *fp_in = open_file(f_in, "r");

    /* The Mode Bits for Access Permission
     * Ref:
     * https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
     */
    make_dir("tmp", S_IRWXU);

    size_t i;
    int scan_stat = 0;
    for (i = 0; scan_stat != EOF && i < MAX_FILES; i++) {
        /* Read MAX_CHUNK_SIZE of integers from the input file */
        size_t j;
        for (j = 0; j < MAX_CHUNK_SIZE; j++) {
            /* Checks EOF */
            if ((scan_stat = fscanf(fp_in, "%d", &n[j])) == EOF)
                break;
        }
        if (j == 0)
            break;

        merge_sort(n, 0, j - 1);

        char f_out[MAX_FILE_NAME_LENGTH];
        sprintf(f_out, "tmp/%lu.txt", i + 1);

        fp[i] = open_file(f_out, "w");

        /* Writes the sorted array n to f_out */
        size_t int_count = j;
        for (j = 0; j < int_count; j++) {
            fprintf(fp[i], "%d\n", n[j]);
        }

        fclose(fp[i]);
    }
    fclose(fp_in);

    end = clock();

    printf("Elapsed Time of split_file_and_sort: %lf secs\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    return i;
}

static void external_merge_sort(const char *f_in) {
    size_t file_count = split_file_and_sort(f_in);

    /* Opens sorted files from tmp directory */
    for (size_t i = 0; i < file_count; i++) {
        char filename[MAX_FILE_NAME_LENGTH];
        sprintf(filename, "tmp/%lu.txt", i + 1);
        fp[i] = open_file(filename, "r");
    }

    heap_t *h = malloc(sizeof(heap_t));
    h->node = malloc((file_count + 1) * sizeof(node_t));
    h->tail = file_count;

    for (int i = 0; i < file_count; i++) {
        fscanf(fp[i], "%d", &h->node[i + 1].value);
        h->node[i + 1].index = i;
    }
    /* Heapsort */
    build_min_heap(h);

    /* Writes the smallest value to the output file */
    FILE *fp_out = open_file("output.txt", "w");
    fprintf(fp_out, "%d\n", get_min_value(h));

    int curr = get_min_index(h);
    while (h->tail > 0) {
        int tmp;
        /* End of the current file */
        if (fscanf(fp[curr], "%d", &tmp) == EOF) {
            /* Updates min_heap */
            swap(&h->node[h->tail--], &h->node[1]);
            if (h->tail == 0)
                break;
        } else {
            h->node[1].value = tmp;
            h->node[1].index = curr;
        }
        min_heapify(h, 1);
        fprintf(fp_out, "%d\n", get_min_value(h));
        curr = get_min_index(h);
    }

    /* Deletes sorted files in tmp directory */

    /* Closes sorted files in tmp directory */
    for (int i = 0; i < file_count; i++)
        fclose(fp[i]);

    fclose(fp_out);

    free(h->node);
    free(h);
}

static void build_min_heap(heap_t *h) {
    for (int i = h->tail / 2; i >= 1; i--)
        min_heapify(h, i);
}

static void min_heapify(heap_t *h, int curr) {
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

static int get_min_value(heap_t *h) { return h->node[1].value; }

static int get_min_index(heap_t *h) { return h->node[1].index; }

static void swap(node_t *n1, node_t *n2) {
    /* Swaps the values */
    n1->value ^= n2->value;
    n2->value ^= n1->value;
    n1->value ^= n2->value;
    /* Swaps the indices */
    n1->index ^= n2->index;
    n2->index ^= n1->index;
    n1->index ^= n2->index;
}

static void merge_sort(int arr[], int start, int end) {
    if (end <= start)
        return;

    int middle = (start + end) >> 1;
    merge_sort(arr, start, middle);
    merge_sort(arr, middle + 1, end);
    merge(arr, start, middle, end);
}

static void merge(int arr[], int start, int middle, int end) {
    size_t len1 = middle - start + 1;
    size_t len2 = end - middle;
    int *a = malloc(len1 * sizeof(int));
    int *b = malloc(len2 * sizeof(int));

    if (!a || !b) {
        puts("Error: malloc failed");
        exit(EXIT_FAILURE);
    }

    size_t count1 = 0, count2 = 0;
    for (int i = start; i <= middle; i++)
        a[count1++] = arr[i];
    for (int i = middle + 1; i <= end; i++)
        b[count2++] = arr[i];

    count1 = count2 = 0;
    for (int i = start; i <= end; i++) {
        if (count1 >= len1) {
            arr[i] = b[count2++];
        } else if (count2 >= len2) {
            arr[i] = a[count1++];
        } else if (a[count1] < b[count2]) {
            arr[i] = a[count1++];
        } else {
            arr[i] = b[count2++];
        }
    }

    free(a);
    free(b);
}
