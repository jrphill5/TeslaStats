#include <stdio.h>

float eqcap( int ser, int par );
float MCC, MCV;

int main()
{

	int ser, par;

	// Individual capacitance and voltage 
	// ratings of MMC component capacitors.
	MCC = 0.15; // microfarads
	MCV = 1200; // volts


	printf("+---------------------------------------------------------------------------+\n");
	printf("|                       Capacitance Calculator v1.00                        |\n");
	printf("|                               Jay Phillips                                |\n");
	printf("+-----+---------+-----------+-----------+-----------+-----------+-----------+\n"); 
	printf("| SER | VOLTAGE |   PAR 1   |   PAR 2   |   PAR 3   |   PAR 4   |   PAR 5   |\n"); 
	printf("+-----+---------+-----------+-----------+-----------+-----------+-----------+"); 
	for ( ser = 1; ser <= 30; ser++ )
	{

		printf("\n| %3d | %7.0f |", ser, ser*MCV);
		for ( par = 1; par <= 5; par++ )
			printf(" %9.1f |", eqcap(ser, par));

	}
	printf("\n+-----+---------+-----------+-----------+-----------+-----------+-----------+\n"); 

	return 0;

}

// Returns equivalent capacitance in picofarads.
float eqcap( int ser, int par )
{

	return par*MCC/ser * 1000.0;

}
