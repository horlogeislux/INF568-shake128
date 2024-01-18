zip: makefile shake128 README.txt
	rm -f hector.zip
	zip -q hector.zip makefile shake128.c README.txt

shake128: shake128.c
	gcc -g -Wall -Wno-unused -Wextra shake128.c -o shake128

.PHONY: clean
clean:
	rm -f shake128
	rm -f hector.zip
