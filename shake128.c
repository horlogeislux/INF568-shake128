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

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

#if DEBUG > 0
#define DBG(s) s
#else
#define DBG(s)
#endif

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
            b[x][y] = rotate_left(a[x][y], rhoOffset[y][x]);
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

static const uint64_t RC[] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808A,
    0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008A,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
    0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
    0x000000000000800A, 0x800000008000000A, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
};

static void iota_k(uint64_t a[5][5], int k) { a[0][0] ^= RC[k]; }

// bin is an array of 25 = 1600/64 elements of 64 bits
static void words_to_state(const uint64_t bin[25], uint64_t a[5][5]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            a[x][y] = bin[5 * y + x];
        }
    }
}

// bin is an array of 25 = 1600/64 elements
static void state_to_words(const uint64_t a[5][5], uint64_t bin[25]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            bin[5 * y + x] = a[x][y];
        }
    }
}

static void disp(uint64_t a[5][5]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            // y are lines, x are columns
            printf("%016lX ", a[y][x]);
        }
        putchar('\n');
    }
}

static void dispwords(uint64_t S[25]) {
    uint8_t* t = (uint8_t*)S;
    for (int n = 0; n < 25 * 8; n++) {
        printf("%02x ", t[n]);
        if ((n + 1) % 32 == 0) putchar('\n');
    }
    putchar('\n');
}

// modifies in place
static void keccak_p(uint64_t bin[25]) {
    DBG(puts("Input of permutation:"); dispwords(bin);)

    uint64_t a[5][5] = {0};
    words_to_state(bin, a);

    // DBG(puts("Input of permutation:"); disp(a);)

    for (int k = 0; k < 24; k++) {
        // DBG(printf("\n--- Round %d ---\n\n", k);)
        theta(a);
        // DBG(puts("After theta:"); disp(a);)

        rho(a);
        // DBG(puts("After rho:"); disp(a);)

        pi(a);
        // DBG(puts("After pi:"); disp(a);)

        chi(a);
        // DBG(puts("After chi:"); disp(a);)

        iota_k(a, k);
        // DBG(puts("After iota:"); disp(a);)
    }

    // DBG(puts("State after permutation:"); disp(a);)
    state_to_words(a, bin);

    DBG(puts("State after permutation:"); dispwords(bin);)
}

static int sponge(unsigned long fsize, unsigned long d) {
    // assume the message is byte aligned: len_N = 8m

    const unsigned b = 1600;     // internal data in bits
    const unsigned bd8 = b / 8;  // internal data in octets

    const unsigned r = 1344;     // rate in bits
    const unsigned rd8 = r / 8;  // rate in octets

    const unsigned c = b - r;    // capacity in bits
    const unsigned cd8 = c / 8;  // capacity in octets

    // by definition, 0 <= % < rd8, so q > 0
    const unsigned q = rd8 - ((fsize / 8) % rd8);
    printf("q %d\n", q);

    uint8_t* bin = malloc(fsize + q);
    memset(bin, 0, fsize + q);
    long fsize2 = fread(bin, 1, fsize, stdin);

    if (fsize2 < 0 || (unsigned long)fsize2 != fsize) {
        free(bin);
        fprintf(stderr, "bad read: %ld %ld\n", fsize, fsize2);
        return EXIT_FAILURE;
    }

    bin[fsize] = 0b00011111;
    bin[fsize + q - 1] |= 0b10000000;

    // message P: rd8 divides fsize + q
    uint64_t* bin64 = malloc(fsize + q);
    memset(bin64, 0, fsize + q);
    memcpy(bin64, bin, fsize + q);

    uint64_t S[25] = {0};
    uint8_t* S8 = (uint8_t*)S;  // 200 octets
    // nblocks = len P in octets (divided by) rate in octets
    const unsigned nblocks = (fsize + q) / rd8;

    for (unsigned ni = 0; ni < nblocks; ni++) {
        for (unsigned j = 0; j < 21; j++) {
            S[j] ^= bin64[j + ni * rd8];
        }
        // for (unsigned j = 20; j < 24; j++) S[j] ^= 0;
        keccak_p(S);
    }

    printf("--- Switching to squeezing phase --- %ld\n", d);

    uint8_t* Z = malloc(d);
    for (unsigned long di = 0; di < d; di++) {
        // squeeze / extract
        Z[di] = S8[di % 200];

        if (((di + 1) % 200 == 0)) {
            keccak_p(S);
        }
    }

    puts("\nhash:");
    for (unsigned long di = 0; di < d; di++) {
        printf("%x", Z[di]);
    }
    puts("");

    free(Z);
    free(bin64);
    free(bin);
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (2 != argc) {
        fprintf(stderr,
                "Error: expected exactly one argument (integer >= 0, bytes)\n");
        return EXIT_FAILURE;
    }

    char* endptr = NULL;
    long d = strtol(argv[1], &endptr, 10);  // in octets

    if (*endptr != '\0' || !isdigit(argv[1][0]) || d < 0) {
        fprintf(stderr, "Error: expected integer >= 0, bytes\n");
        return EXIT_FAILURE;
    }

    // freopen(NULL, "rb", stdin);  // TODO check

    fseek(stdin, 0, SEEK_END);
    long fsize = ftell(stdin);
    rewind(stdin);
    printf("fsize %lu\n", fsize);

    sponge((unsigned long)fsize, (unsigned long)d);

    return EXIT_SUCCESS;
}
