#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    srand(time(NULL));
    long long int n;

    if (argc > 2) {
        puts("ERROR: Too many arguments");
        exit(EXIT_FAILURE);
    }

    if (argc == 1)
        n = 3e9;    /* Default value */
    else if (argc == 2)
        sscanf(argv[1], "%lld", &n);
    
    char filename[] = "input.txt";
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
        printf("%lld\n", i);
    }
    end = clock();
    printf("Done!\n\nTotal Time: %lf secs\n", (double)(end - start) / CLOCKS_PER_SEC);

    fclose(fp);

    return 0;
}