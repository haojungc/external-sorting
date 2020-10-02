#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILENAME_LENGTH 30

static void swap(int *a, int *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

int main(int argc, char **argv) {
    char filename[MAX_FILENAME_LENGTH] = "output.txt";

    if (argc == 2)
        sscanf(argv[1], "%s", filename);

    FILE *fp = fopen(filename, "r");

    if (!fp) {
        printf("Error: Unable to open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    long long int count = 0;
    int tmp1, tmp2;
    if (fscanf(fp, "%d", &tmp1) == EOF) {
        puts("Error: empty file");
        exit(EXIT_FAILURE);
    }
    count++;
    bool is_ascending = true;
    while ((fscanf(fp, "%d", &tmp2) != EOF)) {
        if (tmp2 < tmp1)
            is_ascending = false;
        swap(&tmp1, &tmp2);
        count++;
    }
    if (is_ascending)
        printf("Success: %s is in ascending order\n", filename);
    else
        puts("Error: not in ascending order");

    printf("Total numbers: %lld\n", count);

    fclose(fp);

    return 0;
}