/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * StegoExtract.c: A program for manipulating images                           *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include <string.h>
#include "image.h"

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d\n"

#define BYTETOBINARY(byte) \
(byte & 0x80 ? 1 : 0), \
(byte & 0x40 ? 1 : 0), \
(byte & 0x20 ? 1 : 0), \
(byte & 0x10 ? 1 : 0), \
(byte & 0x08 ? 1 : 0), \
(byte & 0x04 ? 1 : 0), \
(byte & 0x02 ? 1 : 0), \
(byte & 0x01 ? 1 : 0)

#define PRINTBIN( x ) printf(BYTETOBINARYPATTERN, BYTETOBINARY(x));

#define BYTE 8

unsigned char getlsbs(unsigned char *p);

int main(int argc, char *argv[])
{  
  int i, j, k, cover_bits, bits;
  struct Buffer b = {NULL, 0, 0};
  struct Image img = {0, NULL, NULL, NULL, NULL, 0, 0};
  byte b0;
   
  if (argc != 3) 
    {
      printf("\n%s <stego_file> <file_to_extract> \n", argv[0]);
      exit(1);
    }
  ReadImage(argv[1],&img);       // read image file into the image buffer img
                                 // the image is an array of unsigned chars (bytes) of NofR rows
                                 // NofC columns, it should be accessed using provided macros

  // hidden information 
  // first four bytes is the size of the hidden file
  // next 4 bytes is the G number (4 bits per digit)
  if (!GetColor)
    cover_bits = img.NofC*img.NofR;
  else 
    cover_bits = 3*img.NofC*img.NofR;    

  b.size = 0;
  // extract four size bytes for the Buffer's size field
  // Set this to b.size

  unsigned char *p = NULL; //initializes an arrray of unsigned chars
  p = malloc(BYTE * sizeof(unsigned char));
  if(p == NULL)    //allocation check
  {
	printf("Array not allocated.\n");
	exit(1);
  }

  int temp = 0; //initializes temporary variable for bit shifting

  //iterates over the first 32 bytes to extract their LSBs
  for(i=0; i<BYTE/2; i++)
  {
	for(j=(i*BYTE); j<((i+1)*BYTE); j++) //stores next 8 bytes from image in array
	{
		p[j%BYTE] = GetGray(j);
	}

	//Extracts LSBs from 8 bytes in array
	b0 = getlsbs(p);
	temp = b0;

	//assigns b0 to b.size
	temp <<= i*BYTE;
	b.size |= temp;
  }

  printf("b.size: %d\n", b.size);

  b.data = malloc(b.size); // Allocates room for the output data file
  if(b.data == NULL)
  {
	printf("b.data not allocated.\n");
	exit(1);
  }
 
  // extract the eight digits of your G# using 4 bits per digit

  unsigned char gnum[8] = {0,0,0,0,0,0,0,0}; //initializes Gnumber
  unsigned char shift = 0; //initialize temporary variable for bit shifting
  int count = 0; //initializes counter

  printf("G#: ");

  //iterates over the second 32 bytes to extract their LSBs
  for(i=i; i<BYTE; i++)
  {
	for(j=(i*BYTE); j<((i+1)*BYTE); j++) //stores next 8 bytes from image in array
	{
		p[j%BYTE] = GetGray(j);
	}

	b0 = getlsbs(p); //extracts LSBs from 8 bytes in array
	shift = b0;

	//uses first 4 bits in Gnumber
	b0 >>= BYTE/2;
	gnum[count] = b0;
	printf("%d", gnum[count]);
	count++;

	//uses second 4 bits for Gnumber
	shift <<= BYTE/2;
	shift >>= BYTE/2;
	gnum[count] = shift;
	printf("%d", gnum[count]);
	count++;
  }
  
  printf("\n");

  // here you extract information from the image one byte at the time
  // note that you should extract only the least significant bits of the image
  
  temp = BYTE*8; //temporary variable to hold current location in the cover image
  for (i=0; i<b.size; i++) //iterates one byte at a time to create the recovery file
  {
	for(j=temp; j<((b.size*BYTE)+64); j++) //stores the next 8 bytes of the cover image in the array
	{
		p[j%BYTE] = GetGray(j);

		if(j%BYTE == 7)
			break;
	}

	//Extracts LSB from each byte in the array and sets it in the recovery file
	b0 = getlsbs(p);
	SetByte(i, b0);

	//updates location in cover image
	j++;
	temp = j;
  }

  WriteBinaryFile(argv[2],b);  // output payload file

  free(p);
  free(b.data);
}

//Extracts embedded bits and returns the data those bits create
unsigned char getlsbs(unsigned char *p)
{
	int i = 0; //initialize iterator
	unsigned char b0 = 0x0; //initialize return value

	//iterates over the given array of unsigned chars
	//if the unsigned char is even, perform 'bitwise and' operation on return value with 01111111
	//otherwise, perform 'bitwise or' operation on return value with 10000000
	//then shifts the return value to the left by 1.
	for(i=0; i<BYTE; i++)
	{
		if(p[i]%2 == 0)
			b0 &= 0x7F;
		else
			b0 |= 0x80;

		if((i+1) != BYTE)
			b0 >>= 1;
	}

	return b0;
}
