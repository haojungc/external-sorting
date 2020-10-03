#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILENAME_LENGTH 30
#define MAX_FILES 1000
#define MAX_CHUNK_SIZE 1000000

static FILE *open_file(const char *filename, const char *mode);
static void make_dir(const char *dir, mode_t mode);
static void external_sort(const char *);
static size_t split_file_and_sort(const char *);
static size_t merge_files(size_t total_files);
static void merge_sort(int[], int, int);
static void merge(int[], int, int, int);

FILE *fp[MAX_FILES];
int n[MAX_CHUNK_SIZE];

int main(int argc, char **argv) {
    /* Default file */
    char f_in[MAX_FILENAME_LENGTH] = "input2.txt";

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

    external_sort(f_in);

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

    size_t i, scan_stat = 0;
    for (i = 0; scan_stat != EOF; i++) {
        /* Read MAX_CHUNK_SIZE of integers from the input file */
        int j;
        for (j = 0; j < MAX_CHUNK_SIZE; j++) {
            /* Checks EOF */
            if ((scan_stat = fscanf(fp_in, "%d", &n[j])) == EOF)
                break;
        }
        if (j == 0)
            break;

        merge_sort(n, 0, j - 1);

        char f_out[MAX_FILENAME_LENGTH];
        sprintf(f_out, "tmp/%lu.txt", i + 1);
        FILE *fp_out = open_file(f_out, "w");

        /* Writes the sorted array n to f_out */
        int int_count = j;
        for (j = 0; j < int_count; j++)
            fprintf(fp_out, "%d\n", n[j]);
        fclose(fp_out);
    }
    fclose(fp_in);

    end = clock();
    printf("Elapsed Time of split_file_and_sort: %lf secs\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    return i; /* The number of sorted files */
}

static void external_sort(const char *f_in) {
    size_t total_files = split_file_and_sort(f_in);

    do {
        /* Merges MAX_FILES of sorted files to one file */
        total_files = merge_files(total_files);
    } while (total_files > 1);

    /* Moves the sorted file from tmp/ to current directory */
    char old[] = "tmp/1.txt", new[] = "output.txt";
    rename(old, new);
    rmdir("tmp");
}

static size_t merge_files(size_t total_files) {
    /* Merges MAX_FILES of files */
    for (size_t i = 0; i * MAX_FILES < total_files; i++) {
        /* Opens MAX_FILES of sorted files from tmp directory */
        size_t size = (total_files / MAX_FILES > 0) ? MAX_FILES
                                                    : (total_files % MAX_FILES);
        for (size_t j = 0; j < size; j++) {
            char filename[MAX_FILENAME_LENGTH];
            sprintf(filename, "tmp/%lu.txt", i * MAX_FILES + j + 1);
            fp[j] = open_file(filename, "r");
        }

        /* Creates a heap */
        heap_t *h = malloc(sizeof(heap_t));
        h->node = malloc((size + 1) * sizeof(node_t));
        h->tail = size;

        /* Reads the first integers from each file
         * and stores them in the heap */
        for (int j = 0; j < size; j++) {
            fscanf(fp[j], "%d", &h->node[j + 1].value);
            h->node[j + 1].index = j;
        }

        /* Heapsort */
        build_min_heap(h);

        /* Writes the smallest value to the output file
         * and names it "tmp_x.txt" */
        char f_out[MAX_FILENAME_LENGTH];
        sprintf(f_out, "tmp/tmp_%lu.txt", i + 1);
        FILE *fp_out = open_file(f_out, "w");
        fprintf(fp_out, "%d\n", get_min_value(h));

        int curr = get_min_index(h);
        while (1) {
            int tmp;
            /* End of the current file */
            if (fscanf(fp[curr], "%d", &tmp) == EOF) {
                /* Closes the sorted file and deletes it */
                fclose(fp[curr]);
                char filename[MAX_FILENAME_LENGTH];
                sprintf(filename, "tmp/%lu.txt", i * MAX_FILES + curr + 1);
                remove(filename);
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
        fclose(fp_out);
        free(h->node);
        free(h);

        /* Renames "tmp_x.txt" to "x.txt" */
        char old[MAX_FILENAME_LENGTH], new[MAX_FILENAME_LENGTH];
        sprintf(old, "tmp/tmp_%lu.txt", i + 1);
        sprintf(new, "tmp/%lu.txt", i + 1);
        rename(old, new);
    }
    return (total_files - 1) / 1000 + 1;
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
