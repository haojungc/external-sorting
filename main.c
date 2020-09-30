#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_FILE_NAME_LENGTH 30
#define MAX_CHUNK_SIZE 1000000

static size_t split_file(const char *);
static void read_file();
static void external_merge_sort();

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

    size_t file_count = split_file(f_in);

    external_merge_sort();

    end = clock();

    printf("Elapsed Time: %lf secs\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}

static size_t split_file(const char *f_in) {
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

    size_t file_count = 0;
    while (1) {
        char f_out[MAX_FILE_NAME_LENGTH];
        char tmp[20];

        /* Checks EOF */
        int scan_stat = 0;
        if ((scan_stat = fscanf(fp_in, "%s", tmp)) == EOF)
            break;

        sprintf(f_out, "tmp/unsorted_%lu.txt", ++file_count);

        FILE *fp_out = fopen(f_out, "w");
        if (!fp_out) {
            printf("Error: Cannot create file %s\n", f_out);
            exit(EXIT_FAILURE);
        }

        /* Reads integers from f_in and writes to f_out */
        fprintf(fp_out, "%s\n", tmp);
        int count = 1;
        while (count++ < MAX_CHUNK_SIZE &&
               (scan_stat = fscanf(fp_in, "%s", tmp)) != EOF) {
            fprintf(fp_out, "%s\n", tmp);
        }
        fclose(fp_out);
    }
    fclose(fp_in);

    end = clock();

    printf("Elapsed Time of split_file: %lf secs\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    return file_count;
}

static void read_file() {}

static void external_merge_sort() {}
