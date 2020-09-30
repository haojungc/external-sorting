#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILE_NAME_LENGTH 30

int main(int argc, char **argv) {
    srand(time(NULL));

    /* Default values */
    long long int n = 3e9;
    char filename[MAX_FILE_NAME_LENGTH] = "input.txt";

    /* Too many arguments */
    if (argc > 3) {
        puts("Error: Too many arguments");
        printf("Usage: ./(program_name) [the-number-of-random-integers] "
               "[name-of-output-file]\n"
               "(default number: %g, default name: %s)\n",
               (double)n, filename);
        exit(EXIT_FAILURE);
    }

    /* Updates default values */
    if (argc > 1) {
        sscanf(argv[1], "%lld", &n);
        if (argc == 3)
            sscanf(argv[2], "%s", filename);
    }

    /*
     * The return value of access() is 0 if the access is permitted, and -1
     * otherwise.
     * Ref:
     * https://www.gnu.org/software/libc/manual/html_node/Testing-File-Access.html
     */
    if (access(filename, F_OK) == 0) {
        printf("Warning: %s already exists\n", filename);
        puts("Continue? (y: overwrite, n: abort)");
        char c;
        scanf("%c", &c);
        if ((c | ' ') != 'y')
            exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(filename, "w");
    int rand_num, sign;
    clock_t start, end;

    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    printf("%s created\n"
           "Generating %lld random integers...\n",
           filename, n);
    start = clock();

    /* Generates random integers */
    for (long long int i = 0; i < n; i++) {
        rand_num = rand();
        sign = rand() << 31;
        rand_num |= sign;
        fprintf(fp, "%d\n", rand_num);
    }
    end = clock();

    puts("Done!\n---");
    printf("Elapsed Time: %lf secs\n", (double)(end - start) / CLOCKS_PER_SEC);

    fclose(fp);

    return 0;
}