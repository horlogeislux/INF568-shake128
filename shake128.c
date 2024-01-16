#include <stdio.h>

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

unsigned long rotate_right(unsigned long x, unsigned d) {
    return (x >> d) | (x << (64 - d));
}

unsigned long rotate_left(unsigned long x, unsigned d) {
    return (x << d) | (x >> (64 - d));
}

void theta(unsigned long** a) {}
void rho(unsigned long** a) {}
void pi(unsigned long** a) {}
void chi(unsigned long** a) {}
void iota_k(unsigned long** a, int k) {}

int main(int argc, char** argv) {
    printf("argc:%d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv(%d):%s\n", i, argv[i]);
    }
    puts("hello world!");

    return 0;
}

/*
+++ The round constants +++

RC[00][0][0] = 0000000000000001
RC[01][0][0] = 0000000000008082
RC[02][0][0] = 800000000000808A
RC[03][0][0] = 8000000080008000
RC[04][0][0] = 000000000000808B
RC[05][0][0] = 0000000080000001
RC[06][0][0] = 8000000080008081
RC[07][0][0] = 8000000000008009
RC[08][0][0] = 000000000000008A
RC[09][0][0] = 0000000000000088
RC[10][0][0] = 0000000080008009
RC[11][0][0] = 000000008000000A
RC[12][0][0] = 000000008000808B
RC[13][0][0] = 800000000000008B
RC[14][0][0] = 8000000000008089
RC[15][0][0] = 8000000000008003
RC[16][0][0] = 8000000000008002
RC[17][0][0] = 8000000000000080
RC[18][0][0] = 000000000000800A
RC[19][0][0] = 800000008000000A
RC[20][0][0] = 8000000080008081
RC[21][0][0] = 8000000000008080
RC[22][0][0] = 0000000080000001
RC[23][0][0] = 8000000080008008

+++ The rho offsets +++

RhoOffset[0][0] =  0
RhoOffset[1][0] =  1
RhoOffset[2][0] = 62
RhoOffset[3][0] = 28
RhoOffset[4][0] = 27
RhoOffset[0][1] = 36
RhoOffset[1][1] = 44
RhoOffset[2][1] =  6
RhoOffset[3][1] = 55
RhoOffset[4][1] = 20
RhoOffset[0][2] =  3
RhoOffset[1][2] = 10
RhoOffset[2][2] = 43
RhoOffset[3][2] = 25
RhoOffset[4][2] = 39
RhoOffset[0][3] = 41
RhoOffset[1][3] = 45
RhoOffset[2][3] = 15
RhoOffset[3][3] = 21
RhoOffset[4][3] =  8
RhoOffset[0][4] = 18
RhoOffset[1][4] =  2
RhoOffset[2][4] = 61
RhoOffset[3][4] = 56
RhoOffset[4][4] = 14
*/