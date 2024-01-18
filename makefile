CFLAGS=-funroll-loops -march=native -Wall -Wextra

shake128: shake128.c
	gcc -O3 $(CFLAGS) shake128.c -o shake128
	#gcc -pg $(CFLAGS) shake128.c -o shake128

zip: makefile shake128 README.txt
	rm -f hector.zip
	zip -q hector.zip makefile shake128.c README.txt

.PHONY: clean
clean:
	rm -f shake128
	rm -f hector.zip
