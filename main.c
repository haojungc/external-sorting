#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_FILE_NAME_LENGTH 30
#define MAX_FILES 1000000
#define MAX_CHUNK_SIZE 10

static size_t split_file_and_sort(const char *);
static void read_file();
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

static size_t split_file_and_sort(const char *f_in) {
    clock_t start, end;
    start = clock();

    FILE *fp_in = fopen(f_in, "r");

    if (!fp_in) {
        printf("Error: Cannot open file %s\n", f_in);
        exit(EXIT_FAILURE);
    }

    /* Checks if directory tmp exists */
    struct stat st;
    if (stat("tmp", &st) == -1) {
        /* The Mode Bits for Access Permission
         * Ref:
         * https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
         */
        int mkdir_stat = mkdir("tmp", S_IRWXU);
        if (mkdir_stat == -1) {
            puts("Error: Cannot create directory tmp");
            exit(EXIT_FAILURE);
        }
    }

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

        fp[i] = fopen(f_out, "w");
        if (!fp[i]) {
            printf("Error: Cannot create file %s\n", f_out);
            exit(EXIT_FAILURE);
        }

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

static void read_file() {}

static void external_merge_sort(const char *f_in) {
    size_t file_count = split_file_and_sort(f_in);
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
