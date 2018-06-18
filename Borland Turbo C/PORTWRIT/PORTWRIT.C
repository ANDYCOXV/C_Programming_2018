/*
Author: Andy Cox V
Date: 8/21/2016
portwrit.c --> portwrit.exe
Description:
	This program will read bytes from a file,
then output these bytes to port 0x378 (LPT) for an
extended period of time, defined by the sample rate integer.
*/

#include <stdio.h>
#include <dos.h>

unsigned long int size = 0;
long double sample = 0;
unsigned char byte = 0;

FILE *file;

int main(int argc, char *argv[])
{

	printf("\nAndy Cox V - 8/21/2016 - portwrit.exe - v1.0\n");
	printf("Usage: portwrit.exe [filename] [port speed in hertz]\n");

	file = fopen(argv[1], "r");

	if((!file) || (argc != 3))
	{
		printf("\nERROR: File not found, or incorrect parameters!");
		return 0;
		}

	sample = atoi(argv[2]);
	sample = ((1/sample)*1000); /* Convert sample rate to Milliseconds */

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	while(size != 0)
	{
		byte = fgetc(file);
		outportb(0x378, byte);
		printf("%02X  ", byte);
		delay(sample);
		size--;
		}

	printf("\n\nDONE!\n");

return 0;
}