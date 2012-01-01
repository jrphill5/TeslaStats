#define AUTHOR  "Jay Phillips"
#define NAME    "MMC Capacitance Calculator"
#define VERSION "1.05"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include "center.c"
#include "SI.c"

// Declare external centering function.
extern void center( char* begin, char* text, int col, char pad, char* end );

// Calculate the equivalent capacitance of an MMC with
// ser series capacitors and par parallel strands.
float eqcap( int ser, int par );

// Hold the capacitance and voltage values of an individual capacitor.
float MCC, MCV;

// Return the SI unit autoscale factor and prefix of a given value.
extern double SIfactor( double value );
extern char SIprefix( double value );

int main()
{

	// Holds information about program.
	char name[strlen(NAME)+strlen(VERSION)+2];

	// Hold number of capacitors in series and strands in parallel.
	int ser, par;

	// Hold information pertaining to the target specs of the MMC.
	float TC, TV, tol;

	// Define values for the parameters of the MMC.
	MCC = 0.15e-6; MCV = 1200;
	TC = 14.31e-9;  TV = 9000;
	tol = 1.0e-9;

	// Holds information about terminal size.
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	// Hold information about table output size.
	int row = w.ws_row - 11;
	int col = ( w.ws_col - 17 ) / 11;
	int width = 11*col+16;

	// Hold all voltage, capacitance, and delta values for MMC.
	float MMCV[row], MMCC[row][col], del;

	// Hold information about smallest MMC possible.
	int MINSER[col]; for ( par = 0; par <= col; par++ ) MINSER[par] = 0;
	
	// Hold closest matches for each parallel strand of capacitors.
	float MINIMUM[col]; for ( par = 0; par <= col; par++ ) MINIMUM[par] = 1.0;

	// Calculate voltages and equivalent capacitances of MMC.
	for ( ser = 0; ser <= row; ser++ )
	{

		if ( ser != row && ser != 0 ) MMCV[ser] = ser*MCV;
		for ( par = 1; par <= col; par++ )
			if ( ser != row && ser != 0 )
			{

				MMCC[ser][par] = eqcap(ser, par);
				del = fabs( MMCC[ser][par] - TC );
				// Determine optimum size of MMC array.
				if ( MMCV[ser] >= TV && del <= tol && del < MINIMUM[par] )
				{

					MINIMUM[par] = del;
					MINSER[par] = ser;

				}

			}

	}

	// Print tabulated calculated values to stdout.
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
		else printf("\n| %3d | %6.2f%cV |", ser, MMCV[ser]*SIfactor(MMCV[ser]), SIprefix(MMCV[ser]));

		for ( par = 1; par <= col; par++ )
			if ( ser == row || ser == 0 ) printf("----------+");
			else
			{

				if ( fabs( MMCC[ser][par] - TC ) <= tol && MMCV[ser] >= TV )
					if ( MINSER[par] == ser ) printf(" [01;32m");
					else printf(" [01;31m");
				else printf(" ");
				printf("%6.2f%cF[0m |", MMCC[ser][par]*SIfactor(MMCC[ser][par]), SIprefix(MMCC[ser][par]));

			}

	}

	//Print optimal specs for MMC array to stdout.
	char string[50];
	for ( par = 0; par <= col; par++ )
		if ( MINSER[par] != 0 )
		{

			float MINC = eqcap( MINSER[par], par );
			float MINV = MINSER[par] * MCV;
			sprintf(string, "%d parallel x %d series array", par, MINSER[par]);
			center("\n|",string,width,' ',"|");
			sprintf(string, "%dx %0.2f%cF/%0.2f%cV capacitors", MINSER[par]*par, MCC*SIfactor(MCC), SIprefix(MCC), MCV*SIfactor(MCV), SIprefix(MCV));
			center("\n|",string,width,' ',"|");
			sprintf(string, "MMC rated at %0.2f%cF/%0.2f%cV", MINC*SIfactor(MINC), SIprefix(MINC), MINV*SIfactor(MINV), SIprefix(MINV));
			center("\n|",string,width,' ',"|");
			printf("\n+"); for ( par = 1; par <= width; par++ ) printf("-"); printf("+\n");
			break;

		}

	return 0;

}

float eqcap( int ser, int par )
{

	return par*MCC/ser;

}
