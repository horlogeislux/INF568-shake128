r run: shake128
	./shake128

shake128: shake128.c
	gcc shake128.c -o shake128
	rm -f hector.zip
	zip hector.zip makefile shake128.c README.txt

zip: shake128
	rm -f hector.zip
	zip hector.zip makefile shake128.c README.txt

.PHONY: clean
clean:
	rm -f shake128
	rm -f hector.zip
