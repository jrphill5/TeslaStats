#define AUTHOR  "Jay Phillips"
#define NAME    "Capacitance Calculator"
#define VERSION "1.01"

#include <stdio.h>
#include <string.h>
#include "center.c"
#include <sys/ioctl.h>

extern void center( char* begin, char* text, int col, char pad, char* end );
float eqcap( int ser, int par );
float MCC, MCV;

// Have user enter target voltage and capacitance
// and make program highlight closest match.

int main()
{

	// Holds information about program.
	char name[strlen(NAME)+strlen(VERSION)+2];

	// Hold number of capacitors in series and strands in parallel.
	int ser, par;

	// Individual capacitance and voltage 
	// ratings of MMC component capacitors.
	MCC = 0.15; // microfarads
	MCV = 1200; // volts

	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int row = w.ws_row - 7;
	int col = ( w.ws_col - 17 ) / 12;
	int width = 12*col+15;

	printf("+"); for ( par = 1; par <= width; par++ ) printf("-"); printf("+");
	sprintf(name,"%s v%s",NAME,VERSION);
	center("\n|",name,width,' ',"|");
	center("\n|",AUTHOR,width,' ',"|");
	printf("\n+-----+---------+"); 
	for ( par = 1; par <= col; par++ ) printf("-----------+");
	printf("\n| SER | VOLTAGE |");
	for ( par = 1; par <= col; par++ ) printf("   PAR%2d   |", par);
	for ( ser = 0; ser <= row; ser++ )
	{

		if ( ser == row || ser == 0 ) printf("\n+-----+---------+");
		else printf("\n| %3d | %7.0f |", ser, ser*MCV);
		for ( par = 1; par <= col; par++ )
		{
			if ( ser == row || ser == 0 ) printf("-----------+");
			else printf(" %9.1f |", eqcap(ser, par));

		}

	}

	printf("\n");

	return 0;

}

// Returns equivalent capacitance in picofarads.
float eqcap( int ser, int par )
{

	return par*MCC/ser * 1000.0;

}
