/*
Author: Andy Cox V
Date: 12/22/2016
Programming Language: C (C99)
Arguments: tcc synfrqFF.c --> synfrqFF.exe
Notes:
    This program produces a list of the possible musical frequencies (c0-b8) for a custom synthesizer.
This list is printed out as freqlist.txt.
The musical frequencies will be stored as 16-bit hexadecimal unsigned integers and will be used as a delay reference in hz.
Even though the z80 is a 8-bit processor, numbers 255 and less will generate very low frequencies.
The frequencies are calculated with the equation as follows:
f(x) = f*a^x
Where:
f(x) = Calculated frequency.
a = 2^(1/12).
x = Number of half steps above or below the f (fixed note).
f = fixed note, typically 440hz/A440.
The 440 comes from a musical tuned at A440. The purpose of this program is that these values generated
will be loaded into an array for another program for the z80 microprocessor.
*/

#include <stdio.h>

#define a 1.059463 // Almost equal to 2^(1/12).

FILE *output;

// Input the current key value, output the frequency in hz.
unsigned int keyfrequency(int key)
{

	double true_frequency = 1.0;
	unsigned int result = 0;

	// Here a is to the power of the value of key.
	if(key > 0) // Positive power.
	{
		while(key != 0)
		{
			true_frequency *= a;
			key--;
		}
	}
	else if(key < 0) // Negative power.
	{
		while(key != 0)
		{
			true_frequency /= a;
			key++;
		}
	}

	true_frequency *= 440;

	result = (int)true_frequency;
	true_frequency -= result; // Remove non-decimal part.

	if(true_frequency > 0.4) // Check for rounding.
		result++;

	return result;
}

int main(void)
{
	output = fopen("freqlist.txt", "w");
    
	for(char key = -57 ; key < 51 ; key++) // Generate positions from A440.
		fprintf(output, "%04x ", keyfrequency(key));

	fclose(output);
    
	return 0; 
}