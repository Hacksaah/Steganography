CFLAGS = -Wall -g

all: image.o Stego.o StegoExtract.o Stego StegoExtract

image.o: image.c
	gcc -c image.c

Stego.o: Stego.c
	gcc -c Stego.c

StegoExtract.o: StegoExtract.c
	gcc -c StegoExtract.c

Stego: Stego.c
	gcc -o Stego Stego.o image.o

StegoExtract: StegoExtract.c
	gcc -o StegoExtract StegoExtract.o image.o

clean:
	rm -f image.o Stego.o StegoExtract.o Stego StegoExtract
