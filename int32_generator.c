#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    srand(time(NULL));
    long long int n = 3e9;  /* Default value */

    if (argc > 2) {
        puts("Error: Too many arguments");
        printf("Usage: ./(program_name) [the-number-of-random-integers] (default: %g)\n", (double)n);
        exit(EXIT_FAILURE);
    }
 
    if (argc == 2)
        sscanf(argv[1], "%lld", &n);
    
    char filename[] = "input.txt";

    /* 
     * The return value of access() is 0 if the access is permitted, and -1 otherwise.
     * Ref: https://www.gnu.org/software/libc/manual/html_node/Testing-File-Access.html
     */
    if (access(filename, F_OK) == 0) {
        puts("Warning: input.txt already exists");
        puts("Continue? (y: overwrite, n: abort)");
        char c;
        scanf("%c", &c);
        if (c | ' ' != 'y')
            exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(filename, "w");;
    int rand_num, sign;
    clock_t start, end;

    puts("Generating random integers...");
    start = clock();
    for (long long i = 0; i < n; i++) {
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