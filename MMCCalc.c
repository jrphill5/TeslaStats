#define AUTHOR  "Jay Phillips"
#define NAME    "MMC Capacitance Calculator"
#define VERSION "1.03"

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

	// Hold information pertaining to the target specs of the MMC.
	float TC, TV, error;

	// Hold information about smallest MMC possible.
	int MINIMUM = 0,  MINCAPS = 0;
	int MINSER = 0,   MINPAR = 0;
	float MINC = 0.0, MINV = 0.0;
	
	/*printf("Enter Individual Capacitance (F)  : ");
	scanf("%f",&MCC);
	printf("Enter Individual Voltage Rating (V): ");
	scanf("%f",&MCV);*/
	MCC = 0.00000015; MCV = 1200;

	/*printf("Enter Target Capacitance (F)  : ");
	scanf("%f",&TC);
	printf("Enter Target Voltage Rating (V): ");
	scanf("%f",&TV);*/
	TC = 0.0000000144; TV = 9000; error = 1e-9;

	// Holds information about terminal size.
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	// Hold information about table output size.
	int row = w.ws_row - 11;
	int col = ( w.ws_col - 17 ) / 11;
	int width = 11*col+16;

	// Hold all capacitance and voltage values for MMC.
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
				if ( fabs(MMCC[ser][par] - TC) < error && MMCV[ser] > TV )
				{

					if ( MINIMUM == 0 )
					{
						
						MINIMUM = 1;
						MINSER = ser;
						MINPAR = par;
						MINCAPS = ser*par;
						MINC = MMCC[ser][par];
						MINV = MMCV[ser];
						printf(" [01;32m");

					}
					else printf(" [01;31m");

				}
				else printf(" ");
				printf("%6.2f%cF[0m |", MMCC[ser][par]*SIfactor(MMCC[ser][par]), SIprefix(MMCC[ser][par]));

			}

		}

	}
	char string[50];
	sprintf(string, "%d parallel x %d series array", MINPAR, MINSER);
	center("\n|",string,width,' ',"|");
	sprintf(string, "%dx %0.2f%cF/%0.2f%cV capacitors", MINCAPS, MCC*SIfactor(MCC), SIprefix(MCC), MCV*SIfactor(MCV), SIprefix(MCV));
	center("\n|",string,width,' ',"|");
	sprintf(string, "MMC rated at %0.2f%cF/%0.2f%cV", MINC*SIfactor(MINC), SIprefix(MINC), MINV*SIfactor(MINV), SIprefix(MINV));
	center("\n|",string,width,' ',"|");
	printf("\n+"); for ( par = 1; par <= width; par++ ) printf("-"); printf("+\n");

	return 0;

}

float eqcap( int ser, int par )
{

	return par*MCC/ser;

}
