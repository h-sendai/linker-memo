#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

int main(int argc, char *argv[])
{
    gzFile file = gzopen("file.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "expected gzopen() error (we don't have file.dat)\n");
        exit(1);
    }

    return 0;
}
