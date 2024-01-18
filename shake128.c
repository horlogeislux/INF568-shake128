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

#define DEBUG 0

#if DEBUG > 0
#define DBG(s) s
#else
#define DBG(s)
#endif

static inline uint64_t rotate_left(const uint64_t x, const unsigned d) {
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
            b[x][y] = a[(x + 3 * y) % 5][x];
        }
    }

    memcpy(a, b, sizeof(**a) * 5 * 5);
}

static void chi(uint64_t a[5][5]) {
    uint64_t b[5][5] = {0};

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            uint64_t u = a[x][y];
            uint64_t v = ~a[(x + 1) % 5][y];
            uint64_t w = a[(x + 2) % 5][y];

            b[x][y] = u ^ (v & w);
        }
    }

    memcpy(a, b, sizeof(**a) * 5 * 5);
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

static inline void iota_k(uint64_t a[5][5], const int k) { a[0][0] ^= RC[k]; }

// a state is an array of 25 = 1600/64 elements of 64 bits
static inline void words_to_state(const uint64_t state[25], uint64_t a[5][5]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            a[x][y] = state[5 * y + x];
        }
    }
}

// a state is an array of 25 = 1600/64 elements of 64 bits
static inline void state_to_words(const uint64_t a[5][5], uint64_t state[25]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            state[5 * y + x] = a[x][y];
        }
    }
}

#if DEBUG > 0  // avoid -Wunused-function
static void disp_matrix(const uint64_t a[5][5]) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            // y are lines, x are columns
            printf("%016lX ", a[y][x]);
        }
        putchar('\n');
    }
}

static void disp_state(const uint64_t state[25]) {
    uint8_t* p = (uint8_t*)state;
    for (int n = 0; n < 25 * 8; n++) {
        printf("%02x ", p[n]);
        if (0 == (n + 1) % 32) putchar('\n');
    }
    putchar('\n');
}
#endif

// modifies in place
static void keccak_p(uint64_t state[25]) {
    DBG(puts("Input of permutation:"); disp_state(state);)

    uint64_t a[5][5] = {0};
    words_to_state(state, a);

    // DBG(puts("Input of permutation:"); disp_matrix(a);)

    for (unsigned k = 0; k < 24; k++) {
        // DBG(printf("\n--- Round %d ---\n\n", k);)
        theta(a);
        // DBG(puts("After theta:"); disp_matrix(a);)

        rho(a);
        // DBG(puts("After rho:"); disp_matrix(a);)

        pi(a);
        // DBG(puts("After pi:"); disp_matrix(a);)

        chi(a);
        // DBG(puts("After chi:"); disp_matrix(a);)

        iota_k(a, k);
        // DBG(puts("After iota:"); disp_matrix(a);)
    }

    // DBG(puts("State after permutation:"); disp_matrix(a);)
    state_to_words(a, state);

    DBG(puts("State after permutation:"); disp_state(state);)
}

static int sponge(const size_t fsize, const size_t d) {
    // assume the message is byte aligned: fsize = len_N = 8m

    const unsigned rd8 = 1344 / 8;    // rate in octets
    const unsigned rd64 = 1344 / 64;  // rate in octo words, rate block length

    // by definition, 0 <= % < rd8, so q > 0
    const size_t q = rd8 - (fsize % rd8);
    DBG(printf("q %d\n", q);)

    uint8_t* bin = malloc(fsize + q);
    memset(bin, 0, fsize + q);
    const long fsize2 = fread(bin, 1, fsize, stdin);

    if (fsize2 < 0 || (unsigned long)fsize2 != fsize) {
        free(bin);
        fprintf(stderr, "Error: bad read length %ld %ld\n", fsize, fsize2);
        return EXIT_FAILURE;
    }

    bin[fsize] = 0b00011111;
    bin[fsize + q - 1] |= 0b10000000;

    // nblocks = length of padded msg (divided by) rate in octets
    const unsigned nblocks = (fsize + q) / rd8;
    DBG(printf("nblocks %d\n", nblocks);)

    uint64_t S[25] = {0};
    for (size_t ni = 0; ni < nblocks; ni++) {
        for (size_t j = 0; j < rd64; j++) {
            S[j] ^= ((uint64_t*)bin)[j + ni * rd64];
        }
        keccak_p(S);
    }

    DBG(puts("--- Switching to squeezing phase ---", );)

    uint8_t* Z = malloc(d);
    for (size_t di = 0; di < d; di++) {
        Z[di] = ((uint8_t*)S)[di % rd8];

        if (0 == (di + 1) % rd8) {
            keccak_p(S);
        }
    }

    DBG(puts("\nhash:");)
    for (size_t di = 0; di < d; di++) {
        printf("%x", Z[di]);
    }
    putchar('\n');

    free(Z);
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

    fseek(stdin, 0, SEEK_END);
    long fsize = ftell(stdin);
    rewind(stdin);
    DBG(printf("fsize %lu\n", fsize);)

    return sponge((size_t)fsize, (size_t)d);
}
