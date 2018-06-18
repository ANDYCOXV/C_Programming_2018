/*
Author: Andy Cox V
Date: 2/3/2017
Programming Language: C (C99)
Arguments: tcc resdvcal.c --> resdvcal.exe
Notes:
        This program will calculate the values of r1 and r2 in a resistor voltage divider.
The voltages can be from any six digit (seven character) number from 9999.99 to 00000.1.
Press the <ENTER> key to finilize input, or press the <ESC> key to exit.
*/

#include <stdio.h>
#include <stdlib.h>

double input_to_double(void);

/* This code was ripped from an older program I made called tolprcal.c 
It converts user input to an unsigned double.
stdlib.h is required for atof(). 
It returns -1 on invalid number.*/
double input_to_double(void)
{
char position = 0;
unsigned char user_array[8]; /* Set to 8, because there will be a '.' character used Example 1000.00. */ 
unsigned char kbin = 0;

	/* Code to clear the user's input. */
	for(unsigned char clear = 0 ; clear < 8 ; clear++)
		user_array[clear] = 0;

	do
	{
		kbin = getch();
			
		if((kbin == 0x08) && (position > 0)) /* Backspace ASCII character. */
		{
				position--;
				user_array[position] = 0;
				printf("%c %c", 0x08, 0x08); /* Delete the previous character from console. */
		}
		
		if((((kbin >= '0') && (kbin <= '9')) || (kbin == '.')) && (position < 8))
		{
			user_array[position] = kbin;
			putchar(kbin);
			position++;
		}
                
                if(kbin == 0x1b) /* Exit key pressed. */
                        exit(0);
                
	}while(kbin != 0x0D); /* Enter key ASCII value. */

        /* Most likely numbers above 100 will not be used. */
	if(atof(user_array) <= 9999.99)
		return atof(user_array);
	else
	{
		printf("\nERROR: Number must be 9999.99 or less!");
		return -1;
	}
}

/* This returns how far away the current resistor ratio is from the desidered vout, zero would be spot on. */
double resistor_calculation(unsigned double r1, unsigned double r2, unsigned double vin, unsigned double vout)
{
        return (vout-(vin*(r2/(r1+r2))));
}

void main(void)
{
        
        double vin = 0;
        double vout = 0;
        double r1best = 0;
        double r2best = 0;
        double current_combo = 9999;
        
        /* Standard decade resistance values.
        The list used was from BRANNON ELECTRONICS, INC. 1% resistor values.
        To use a decade resistance values every element can be multiplied by 10.
        In turn 13.3 can be 13.3ohms, 133ohms, 1.33kohms, 13.3kohms, and 1.33mohms*/
        double resistor_array[97] = {10.0, 14.7, 21.5, 31.6, 46.4, 68.1, 10.2, 15.0, 22.1, 32.4, 47.5, 69.8, 10.5, 15.4, 22.6, 33.2, 48.7, 71.5, 10.7, 15.8, 23.2, 34.0, 49.9, 73.2, 11.0, 16.2, 23.7, 34.8, 51.1, 75.0, 11.3, 16.5, 24.3, 35.7, 52.3, 76.8, 11, 5, 16.9, 24.9, 36.5, 53.6, 78.7, 11.8, 17.4, 25.5, 37.4, 54.9, 80.6, 12.1, 17.8, 26.1, 38.3, 56.2, 82.5, 12.4, 18.2, 26.7, 39.2, 57.6, 84.5, 12.7, 18.7, 27.4, 40.2, 59.0, 86.6, 13.0, 19.1, 28.0, 41.2, 60.4, 88.7, 13.3, 19.6, 28.7, 42.2, 61.9, 90.9, 13.7, 20.0, 29.4, 43.2, 63.4, 93.1, 14.0, 20.5, 30.1, 44.2, 64.9, 95.3, 14.3, 21.0, 30.9, 45.2, 66.5, 97.6};
        
        printf("\n\nresdvcal.exe - Andy Cox V - 2/3/2016\n"
                "\tThis program will calculate the nessasary resistor values for a resistor divider.\n"
                "\t\tUsing the equation: Vout = Vin*(r2/r1+r2).\n"
                "\t\tA list of common 1/% resistor values is used for the calculation.");
        
        do{
        printf("\n\tInput the Vin voltage <ECS> to quit, or <ENTER> to finilize: ");
                vin = input_to_double();
        }while(vin == -1);
        
        do{
        printf("\n\tInput the Vout voltage <ECS> to quit, or <ENTER> to finilize: ");
                vout = input_to_double();
        }while(vout == -1);

        for(unsigned char r1 = 0 ; r1 < 97 ; r1++)
        {
                for(unsigned char r2 = 0 ; r2 < 97 ; r2++)
                {
                        if((resistor_calculation(resistor_array[r1],resistor_array[r2],vin,vout) < current_combo) && (resistor_calculation(resistor_array[r1],resistor_array[r2],vin,vout) >= 0))
                        {
                                r1best = resistor_array[r1];
                                r2best = resistor_array[r2];
                                current_combo = resistor_calculation(resistor_array[r1],resistor_array[r2],vin,vout);
                        }
                }
        }
        
        printf("\n\nThe ohmage of r1 should be: %lf\n"
        "The ohmage of r2 should be: %lf\n"
        "The desidered Vout value in volts is: %lf\n"
        "With standard value resistors the Vout would be: %lf\n\n"
        "To note these values can be multiplied by powers of ten and still work.\n" 
        "For the ratio of r1 and r2 create the output voltages.", r1best, r2best, vout, (current_combo+vout));
        
        getch();
        exit(0);
}