/*
Author: Andy Cox V
Date: 11/5/2016
Compiler: tcc ASCII.c --> ASCII.exe
Programming Language: C (C99)
Description:
    This program prints the displayable 8-bit ASCII list.
    This program was written rather quickly.
*/

#include <stdio.h>

void main(void)
{
    printf("char octal decimal hexadecimal\n");

    for(unsigned short ascii = 0x20 ; ascii < 0x100 ; ascii++)
        if(ascii % 147 == 0) /* Fill screen untill new screen rolls over. */
            getch();
        else
            printf("%c %03o %03d %02x\t", ascii, ascii, ascii, ascii);
    
	getch();

    exit(0);
}