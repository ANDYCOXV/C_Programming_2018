/*
Author: Andy Cox V
Date: 3/16/2018
Programming Language: C (C99)
Compiler Arguments: gcc commodoreLogo.c -o commodoreLogo.bin -lm
Description:
	Make sure to use the -lm argument to link the include files to the gcc compiler.
        This program was used to help calculate the volume of the Commodore Computer logo bounded between [-3, 9/4]. 
        This program was written for a calculus project. 
	This program was originally written whith numbers instead of letters for the sectors.
		- Sector 1 = B
		- Sector 2 = A
		- Sector 3 = D
		- Sector 4 = C
		- Sector 5 = E
		- Sector 6 = F
		- Sector 7 = G*/
        
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* --- Constants  --- */

/* --- Extra Information ---
#define WIDTH_OF_SECTOR_1	1.5 -- Sectors 4 and 5 are exact copys of sectors 2 and 1 and are mirrored over the x-axis. --
#define WIDTH_OF_SECTOR_2	2.25
#define WIDTH_OF_SECTOR_3	1.5
#define SLICES_OF_SECTOR_1	WIDTH_OF_SECTOR_1 / DEPTH_OF_FOAM -- All of the slices are calculated from widthOfSector / DEPTH_OF_FOAM. --
#define SLICES_OF_SECTOR_2	WIDTH_OF_SECTOR_2 / DEPTH_OF_FOAM
#define SLICES_OF_SECTOR_3	WIDTH_OF_SECTOR_3 / DEPTH_OF_FOAM
*/

/* Labels for the sectors. */
#define SECTOR_1	'B'
#define SECTOR_2	'A'
#define SECTOR_3	'D'
#define SECTOR_4	'C'
#define SECTOR_5	'E'
#define SECTOR_6	'F'
#define SECTOR_7	'G'

#define DEPTH_OF_FOAM		3.0 / 32.0 /* Thickness of the foam used to create the slices (delta X). */
#define FOAM_MIDPOINT		DEPTH_OF_FOAM / 2.0 /* Used for the intitial foam slice since using midpoints must be used for delta X. */
#define TOP_OF_SECTOR_1		(3 * sqrt(3) - 1) / 4 /* Top line of the triangle. */
#define HEIGHT_OF_SECTOR_1	(3 * sqrt(3) - 2) / 4 /* Heigth of the rectanlge and triangle of sector 1. */
#define UPPER_BOUND_OF_SECTOR_1	((51 * sqrt(3)) + 93) / 52 /* End of the triangle bound. */
#define LOWER_BOUND_OF_SECTOR_1	(((3 * sqrt(3)) + 6) / 4) /* Start of the triangle bound. */
#define UPPER_BOUND_OF_SECTOR_2	((3 * sqrt(3)) + 6) / 4
#define LOWER_BOUND_OF_SECTOR_2 0.75
#define UPPER_BOUND_OF_SECTOR_3	0.75
#define LOWER_BOUND_OF_SECTOR_3 -1.5
#define UPPER_BOUND_OF_SECTOR_4	-1.5
#define LOWER_BOUND_OF_SECTOR_4 -3

/* Calculates the volume of the semicircle. */
double volume(double x) /* Where x is the radius of the semicircle. */
{
	return 0.5 * M_PI * DEPTH_OF_FOAM * x * x;
}

/* Calculates the radius of the triangles in sectors 1 and 7. 
The rectangles sectors 2 and 6 do not need to be calculated since their heigth is constant. */
double radiusOfSector1(double x) /* Where x is the slice number. */
{

	/* Calculate the radius of the cross sections for the triangular portion (NOTE: Each trapezoid is a quadralateral). */
	/*return (TOP_OF_SECTOR_1 - ((((-13 * sqrt(3)) + 26) / 3) * x - 3)) / 2; Top of the triangle - growing line. */
	return ((13 * sqrt(3) - 26) / 6) * x + ((3 * sqrt(3) + 11) / 8);
}

/* Calculates the radius of all of the semicircles for sectors 3 and 5. */
double radiusOfSector3(double x) /* Where x is the slice number. */
{
	return (sqrt(9 - (x * x)) - sqrt(2.25 - (x * x))) / 2;
}

/* Calculates the radius of all of the semicircles for sector 4. */
double radiusOfSector4(double x) /* Where x is the slice number. */
{
	return sqrt(9 - (x * x));
}

/* Midpoint values are calculated to aid in the physical design of the model.
The semicircles center will be alligned to the calculated midpoints. */

/* Calculates midpoint values for sectors 1 and 7. */
double sector1Center(double x)
{
	return ((-13 * sqrt(3) + 26) / 3) * x + ((3 * sqrt(3) - 13) / 4); /* Used for the triangular sectors. */
}

/* Calculates the midpoint of sectors 3 and 5. */
double sector3Center(double x)
{
	return (sqrt(3 - (x * x)) + sqrt(2.25 - (x * x))) / 2;
}

/* Prints the information for the user. */
void display(unsigned int slice, double radius, double xCoordinate, double yCenter) /* Where slice is the slice count and radius is the semicircl's radius. */
{
	printf("%d\t%lf\t%lf\t%lf\t%lf\t%lf\n", slice, radius, (radius * 2), volume(radius), xCoordinate, yCenter);
	return;
}

/* Prints the colum titles for the sector attributes. */
void sectorAttrib(char x)
{
	printf("\nAttributes for Sector %c\nSLICE\tRADIUS\t\tDIAMETER\tVOLUME\t\tXAXIS\t\tYAXISCENTER\n", x);
	return;
}

/* prints the volume of the sector. */
void displayDone(char x, double volume)
{
	printf("\nVolume of Sector %c: %lf\n", x, volume);
	return;
}

int main(void)
{
	double totalVolume = 0;
	double radius = 0;
	double tempVolume = 0;
	double carry = 0; /* Used for sectors that are connected so the domains can carry over appropriatly. */
	unsigned int slice = 0;
	unsigned int totalSlices = 0;

	puts("Commodore Computer Logo Calculus Slicing Project - Andy Cox V - Copyright 2018\n"
		"Semicircles are used for the cross sectional shape.\n"
		"This program calculates the radius, diameter, and volume of each circle.\n"
		"The total volume of the logo will computed using summation.\n"
		"YAXISCENTER is the point on the Y-axis to place the center of the simicircle when constructing the model.");

	/* This goes in a specific order since the sectors are created from left to right and whole pieces are split into sectors.
	Sector 2 preceeds sector 1, since sector 2 and 1 are together, and the lower bounds for sector 1 is based off of sector
	2's upper bound. But since the slices have a depth of 3 / 32 of an inch when sector 2's upperbound is surpassed it
	must carry on into sector 1's lower bound to allow a better approximation of the volume that will be used. 
	Do not use the FOAM_MIDPOINT for loops with carries. */

	/* Sector 2. */
	sectorAttrib(SECTOR_2);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	radius = HEIGHT_OF_SECTOR_1 / 2; /* Since the rectangles heigth is constant no need for iteration. */
	for(double i = LOWER_BOUND_OF_SECTOR_2 + FOAM_MIDPOINT ; i < UPPER_BOUND_OF_SECTOR_2 ; i += DEPTH_OF_FOAM, carry = i)
	{
		slice++;
		display(slice, radius, i, (0.25 + TOP_OF_SECTOR_1) / 2); /* Since it is in a rectangle no need to recalculate midpoint. */
		totalVolume += volume(radius);

	}
	displayDone(SECTOR_2, totalVolume - tempVolume);
	totalSlices += slice;
	slice = 0;
	tempVolume = 0;

	/* Sector 1. */
	sectorAttrib(SECTOR_1);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	for(double i = carry ; i < UPPER_BOUND_OF_SECTOR_1 ; i += DEPTH_OF_FOAM)
	{
		slice++;
		radius = radiusOfSector1(i);
		display(slice, radius, i, sector1Center(i));
		totalVolume += volume(radius);
	}

	displayDone(SECTOR_1, totalVolume - tempVolume);
	totalSlices += slice;
	carry = 0; /* Resetting for sectors 3, 4 and 5. */
	slice = 0;
	tempVolume = 0;

	/* Sector 4. */
	/* Need to setup carry from left to right, keep this carry for both sectors 3 and 5. */
	sectorAttrib(SECTOR_4);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	for(double i = LOWER_BOUND_OF_SECTOR_4 + FOAM_MIDPOINT ; i < UPPER_BOUND_OF_SECTOR_4 ; i += DEPTH_OF_FOAM, carry = i)
	{
		slice++;
		radius = radiusOfSector4(i);
		display(slice, radius, i, 0); /* No need to calculate is already centered. */
		totalVolume += volume(radius);

	}
	displayDone(SECTOR_4, totalVolume - tempVolume);
	totalSlices += slice;
	slice = 0;
	tempVolume = 0;

	/* Sector 3. */
	sectorAttrib(SECTOR_3);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	for(double i = carry ; i < UPPER_BOUND_OF_SECTOR_3 ; i += DEPTH_OF_FOAM)
	{
		slice++;
		radius = radiusOfSector3(i);
		display(slice, radius, i, sector3Center(i));
		totalVolume += volume(radius);

	}
	displayDone(SECTOR_3, totalVolume - tempVolume);
	totalSlices += slice;
	slice = 0;
	tempVolume = 0;

	/* Negate the sectorcenter to aquire appropriate y coordinate values. */

	/* Sector 5. */
	sectorAttrib(SECTOR_5);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	for(double i = carry ; i < UPPER_BOUND_OF_SECTOR_3 ; i += DEPTH_OF_FOAM)
	{
		slice++;
		radius = radiusOfSector3(i);
		display(slice, radius, i, -sector3Center(i));
		totalVolume += volume(radius);

	}
	displayDone(SECTOR_5, totalVolume - tempVolume);
	totalSlices += slice;
	slice = 0;
	tempVolume = 0;

	/* Sector 6. */
	sectorAttrib(SECTOR_6);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	radius = HEIGHT_OF_SECTOR_1 / 2; /* Since the rectangles heigth is constant no need for iteration. */
	for(double i = LOWER_BOUND_OF_SECTOR_2 + FOAM_MIDPOINT ; i < UPPER_BOUND_OF_SECTOR_2 ; i += DEPTH_OF_FOAM, carry = i)
	{
		slice++;
		display(slice, radius, i, (0.25 + TOP_OF_SECTOR_1) / 2); /* Since it is in a rectangle no need to recalculate midpoint. */
		totalVolume += volume(radius);

	}
	displayDone(SECTOR_6, totalVolume - tempVolume);
	totalSlices += slice;
	slice = 0;
	tempVolume = 0;

	/* Sector 7. */
	sectorAttrib(SECTOR_7);
	tempVolume = totalVolume; /* Subtract out the old volume from the new volume to get the current volume of the sector. */
	for(double i = carry ; i < UPPER_BOUND_OF_SECTOR_1 ; i += DEPTH_OF_FOAM)
	{
		slice++;
		radius = radiusOfSector1(i);
		display(slice, radius, i, sector1Center(i));
		totalVolume += volume(radius);
	}

	displayDone(SECTOR_7, totalVolume - tempVolume);
	totalSlices += slice;

	printf("\nTotal Volume: %lf\nTotal Number of Slices: %d\nThickness of slices (Delta X - in inches): %lf\n", totalVolume, totalSlices, DEPTH_OF_FOAM);

	exit(0);
}