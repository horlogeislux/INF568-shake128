#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Your submission must include or build a command-line program called shake128,
which takes one command-line argument: the number of bytes (not bits) to be
output. read from the standard input stream, write the resulting hash value to
standard output.

Input files/streams are binary data
(don't assume the input is a file of literal 1 and 0 characters, for example),
and the output should be an ASCII hexadecimal digest (two characters per byte).

In order:
keccak-p = f is the permutation, with tau, rho, pi, chi, iota_k
keccak[c](N, d) = SPONGE[keccak-p[1600,  24], pad10*1, 1600-c](N,d)
SHAKE128(M,d) = keccak[256](M || 1111, d)
*/

#if 0
static uint64_t rotate_right(uint64_t x, unsigned d) {
    return (x >> d) | (x << (64 - d));
}
#endif

static uint64_t rotate_left(uint64_t x, unsigned d) {
    return (x << d) | (x >> (64 - d));
}

static void theta(uint64_t a[5][5]) {
    uint64_t c[5] = {0};
    uint64_t d[5] = {0};

    for (int x = 0; x < 5; x++) {
        c[x] = a[x][0] ^ a[x][1] ^ a[x][2] ^ a[x][3] ^ a[x][4];
    }

    for (int x = 0; x < 5; x++) {
        d[x] = c[(x + 4) % 5] ^ rotate_left(c[(x + 1) % 5], 1);
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            a[x][y] ^= d[x];
        }
    }
}

// access with rhoOffset[y][x]
static const unsigned int rhoOffset[5][5] = {
    {0, 1, 62, 28, 27},  {36, 44, 6, 55, 20}, {3, 10, 43, 25, 39},
    {41, 45, 15, 21, 8}, {18, 2, 61, 56, 14},
};

static void rho(uint64_t a[5][5]) {
    uint64_t b[5][5] = {0};

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            b[x][y] =
                rotate_left(a[x][y], rhoOffset[y][x]);
        }
    }

    memcpy(a, b, sizeof(**a) * 5 * 5);
}

static void pi(uint64_t a[5][5]) {
    uint64_t b[5][5] = {0};

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            int xp3ym5 = (x + 3 * y) % 5;
            b[x][y] = a[xp3ym5][x];
        }
    }

    memcpy(a, b, sizeof(**a) * 5 * 5);
}

static void chi(uint64_t a[5][5]) {
    uint64_t tmp[5][5] = {0};

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            uint64_t u = a[x][y];
            uint64_t v = ~a[(x + 1) % 5][y];
            uint64_t w = a[(x + 2) % 5][y];

            tmp[x][y] = u ^ (v & w);
        }
    }

    memcpy(a, tmp, sizeof(**a) * 5 * 5);
}

static const uint64_t RC1[] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808A,
    0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008A,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
    0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
    0x000000000000800A, 0x800000008000000A, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
};

static void iota_k(uint64_t a[5][5], int k) { a[0][0] ^= RC1[k]; }

void disp(uint64_t a[5][5]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            // y are lines, x are columns
            printf("%016lX ", a[y][x]);
        }
        putchar('\n');
    }
}

int main(int argc, char** argv) {
#if 0
    printf("argc:%d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv(%d):%s\n", i, argv[i]);
    }
#endif

    if (2 != argc) {
        fprintf(stderr,
                "Error: expected exactly one argument (integer >= 0, bytes)\n");
        return EXIT_FAILURE;
    }

    char* endptr = NULL;
    long d = 8 * strtol(argv[1], &endptr, 10);

    if (*endptr != '\0' || !isdigit(argv[1][0]) || d < 0) {
        fprintf(stderr, "Error: expected integer >= 0, bytes\n");
        return EXIT_FAILURE;
    }

    uint64_t a[5][5] = {0};
    disp(a);

    for (int k = 0; k < 24; k++) {
        printf("\n--- Round %d ---\n\n", k);
        theta(a);
        puts("After theta:");
        disp(a);

        rho(a);
        puts("After rho:");
        disp(a);

        pi(a);
        puts("After pi:");
        disp(a);

        chi(a);
        puts("After chi:");
        disp(a);

        iota_k(a, k);
        puts("After iota:");
        disp(a);
    }

    return EXIT_SUCCESS;
}
