author:
Hector Denis


description:
assignment 1 of INF568
fully written and tested on salle machine's skoda.polytechnique.edu
implementation of shake128 following FIPS 202
https://csrc.nist.gov/pubs/fips/202/final


building:
$ make


running:
$ ./shake128 <N> < filename


where:
N is an integer, zero or more
filename is the file to hash
shake reads from stdin and outputs the hash to stdout


testing:
test files are also included

$ ./shake128 32 < /dev/null
7f9c2ba4e88f827d61604550765853ed73b8093f6efbc88eb1a6eacfa66ef26

$ ./shake128 32 < short-text.txt
ba27cc6a7a85887a1888c0678c05cd7fcf619ed791dce41b7e1a81c280bec8bb

$ ./shake128 32 < short-binary.bin
9b171ccf7ff6b9478ce02a54a5a558dde55febc70e12f0ed402567639e404b74


performance:
I achieved performance similar to openssl.

$ dd if=/dev/random of=rand1G.txt bs=1G count=1
$ make -Bs; time ./shake128 32 < rand1g.txt
$ time openssl shake128 rand1g.txt

sample:
[skoda ~/Documents/acry/a1]$ time openssl shake128 rand1g.txt
SHAKE128(rand1g.txt)= 8c176c4be4e28cd1a81468afc381ae8e

real	0m1.936s
user	0m1.818s
sys	0m0.116s
[skoda ~/Documents/acry/a1]$ make -Bs; time ./shake128 32 < rand1g.txt
8c176c4be4e28cd1a81468afc381ae8e874a4ba197017509c6848245bc9579

real	0m1.908s
user	0m1.771s
sys	0m0.134s
