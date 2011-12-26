#define AUTHOR  "Jay Phillips"
#define NAME    "MMC Capacitance Calculator"
#define VERSION "1.02"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include "center.c"
#include "SI.c"

extern void center( char* begin, char* text, int col, char pad, char* end );
float eqcap( int ser, int par );
float MCC, MCV;

// Return the SI unit autoscale factor and prefix of a given value.
extern double SIfactor( double value );
extern char SIprefix( double value );

// Have user enter target voltage and capacitance
// and make program highlight closest match.

int main()
{

	// Holds information about program.
	char name[strlen(NAME)+strlen(VERSION)+2];

	// Hold number of capacitors in series and strands in parallel.
	int ser, par;

	float TC, TV, error;
	
	/*printf("Enter Individual Capacitance (F)  : ");
	scanf("%f",&MCC);
	printf("Enter Individual Voltage Rating (V): ");
	scanf("%f",&MCV);*/
	MCC = 0.00000015; MCV = 1200;

	/*printf("Enter Target Capacitance (F)  : ");
	scanf("%f",&TC);
	printf("Enter Target Voltage Rating (V): ");
	scanf("%f",&TV);*/
	TC = 0.0000000144; TV = 12000; error = 1e-9;

	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int row = w.ws_row - 7;
	int col = ( w.ws_col - 17 ) / 11;
	int width = 11*col+16;

	float MMCC[row][col], MMCV[row];

	printf("+"); for ( par = 1; par <= width; par++ ) printf("-"); printf("+");
	sprintf(name,"%s v%s",NAME,VERSION);
	center("\n|",name,width,' ',"|");
	center("\n|",AUTHOR,width,' ',"|");
	printf("\n+-----+----------+"); 
	for ( par = 1; par <= col; par++ ) printf("----------+");
	printf("\n| SER | V RATING |");
	for ( par = 1; par <= col; par++ ) printf("  PAR%3d  |", par);
	for ( ser = 0; ser <= row; ser++ )
	{

		if ( ser == row || ser == 0 ) printf("\n+-----+----------+");
		else
		{
			
			MMCV[ser] = ser*MCV;
			printf("\n| %3d | %6.2f%cV |", ser, MMCV[ser]*SIfactor(MMCV[ser]), SIprefix(MMCV[ser]));

		}
		for ( par = 1; par <= col; par++ )
		{
			if ( ser == row || ser == 0 ) printf("----------+");
			else
			{

				MMCC[ser][par] = eqcap(ser, par);
				if ( fabs(MMCC[ser][par] - TC) < error && MMCV[ser] > TV ) printf(" [01;31m");
				else printf(" ");
				printf("%6.2f%cF[0m |", MMCC[ser][par]*SIfactor(MMCC[ser][par]), SIprefix(MMCC[ser][par]));

			}

		}

	}

	printf("\n");

	return 0;

}

float eqcap( int ser, int par )
{

	return par*MCC/ser;

}
