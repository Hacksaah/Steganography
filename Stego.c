/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * Stego.c: A program for manipulating images                           *
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

#define PRINTBIN(x) printf(BYTETOBINARYPATTERN, BYTETOBINARY(x));

#define BYTE 8

void setlsbs(unsigned char *p, unsigned char b0);
unsigned char concat(unsigned char a, unsigned char b);

int main(int argc, char *argv[])
{  
  int i, j, k, cover_bits, bits;
  struct Buffer b = {NULL, 0, 0};
  struct Image img = {0, NULL, NULL, NULL, NULL, 0, 0};
  byte b0;
   
  if (argc != 4) 
    {
      printf("\n%s <cover_file> <stego_file> <file_to_hide> \n", argv[0]);
      exit(1);
    }
  ReadImage(argv[1],&img);       // read image file into the image buffer img
                                 // the image is an array of unsigned chars (bytes) of NofR rows
                                 // NofC columns, it should be accessed using provided macros
  ReadBinaryFile(argv[3],&b);    // Read binary data
 

  // hidden information 
  // first four bytes is the size of the hidden file
  // next 4 bytes is the G number (4 bits per digit)
  if (!GetColor)
    cover_bits = img.NofC*img.NofR;
  else 
    cover_bits = 3*img.NofC*img.NofR;    
  bits = (8 + b.size)*8;
  if (bits > cover_bits)
    {
      printf("Cover file is not large enough %d (bits) > %d (cover_bits)\n",bits,cover_bits);
      exit(1);
    }

  // embed four size bytes for the Buffer's size field

  unsigned char *p = NULL;			 //initialize an array of unsigned chars
  p = malloc(BYTE * sizeof(unsigned char));
  if(p == NULL)					 //allocation check
  {
	printf("Array not allocated.\n");
	exit(1);
  }

  int temp = b.size; //initialize variable to hold b.size while the bits get shifted

  printf("b.size: %d\n", b.size); 

  //embeds 32 bits from b.size into the LSBs of the first 32 bytes of the cover image 
  for(i=0; i<BYTE/2; i++)
  {
	b0 = temp; //casts int to byte

	//puts 8 bytes from the cover image into the array
	for(j=(i*BYTE); j<((i+1)*BYTE); j++)
	{
		p[j%BYTE] = GetGray(j);
	}
	
	setlsbs(p,b0); //embeds LSBs
	
	//sets the new bytes back into the cover image
	for(j=(i*BYTE); j<((i+1)*BYTE); j++)
	{
		SetGray(j, p[j%BYTE]);
	}

	temp >>= BYTE; //shift b.size by 8 bits
  }

  // embed the eight digits of your G# using 4 bits per digit
  
  unsigned char gnum[8] = {0,0,9,6,8,8,2,0};  //initialize Gnumber
  int count = 0; //initialize counter to print Gnumber

  printf("G#: ");
  for(count=0; count<8; count++)
  {
	printf("%d", gnum[count]);
  }
  printf("\n");

  //embeds 32 bits from the Gnumber into the second 32 bytes of the cover image
  for(i=i; i<BYTE; i++)
  {
	//puts 8 bytes from the cover image into the array
	for(j=(i*BYTE); j<((i+1)*BYTE); j++)
	{
		p[j%BYTE] = GetGray(j);
	}

	//uses 2 numbers to make 8 bits to embed; numbers are chosen based on value of i
	switch(i)
	{
		case 4:
			b0 = concat(gnum[0], gnum[1]);
			break;
		case 5:
			b0 = concat(gnum[2], gnum[3]);
			break;
		case 6:
			b0 = concat(gnum[4], gnum[5]);
			break;
		case 7:
			b0 = concat(gnum[6], gnum[7]);
			break;
	}

	setlsbs(p, b0); //changes the LSBs of the bytes in the array

	//sets the new bytes back into the cover image
	for(j=(i*BYTE); j<((i+1)*BYTE); j++)
	{
		SetGray(j, p[j%BYTE]);
	}
  }

  //embed payload into cover image here

  temp = BYTE*8;  //temporary variable to hold current location in the cover image

  for(i=0; i<b.size; i++) //used each byte of the payload individually
  {
	for(j=temp; j<((b.size*BYTE)+64); j++) //stores the next 8 bytes of the cover image in the array
	{
		p[j%BYTE] = GetGray(j);
		
		if(j%BYTE == 7)
			break;
	}

	//embeds the next byte in the payload into the next 8 bytes of the cover image
	b0 = GetByte(i); 
	setlsbs(p, b0);

	for(j=temp; j<((b.size*BYTE) + 64); j++) //sets the newly embedded bytes back in the cover image
	{
		SetGray(j, p[j%BYTE]);

		if(j%BYTE == 7)
		{
			j++;
			break;
		}
	}

	temp = j; //updates placeholder in cover image
  }

  free(p);

  WriteImage(argv[2],img);  // output stego file (cover_file + file_to_hide)
}

//Sets the Least Significant Bits of each byte in *p with b0
void setlsbs(unsigned char *p, unsigned char b0)
{
	int i = 0; //initializes iterator
	unsigned char temp; //declares tempory byte

	//shifts b[i] bit all the way to the left, then all the way to the right
	//if the number is zero, p[i] byte goes through 'bitwise and' operation with 11111110
	//other wise, p[i] byte goes through 'bitwise or' operation with 00000001 
	for(i=0; i<BYTE; i++)
	{
		temp = b0 << (BYTE - (i+1));
		temp >>= (BYTE - 1);

		if(temp == 0)
			p[i] &= 0xFE;
		else
			p[i] |= 0x01;
	}
}

//Concatanates the 4 LSBs of each of the given unsigned chars
unsigned char concat(unsigned char a, unsigned char b)
{
	a <<= BYTE/2;
	return a | b;
}
