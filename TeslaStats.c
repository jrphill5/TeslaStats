#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>

double PI, PHI, C0, U0, E0;

float AWG( int gauge );
void divider( char* begin, char* string, char* pad, char* end );

int main()
{

	int i;
	float NSTVI, NSTII, NSTVO, NSTIO, NSTVA, NSTTR, NSTPF;
	float NSTZ, NSTC, LTRC, NSTF, NSTCR, NSTLR;
	float PRIWG, PRIWD, PRID1, PRID2, PRIF, PRILN, PRIL, PRIN;
	float SECWG, SECWD, SECD,  SECH,  SECF, SECLN, SECL, SECN;
	float TOPD, TOPC;
	float ARCLN;

	PI  = 3.1415926535897932384626433832795;
	PHI = 0.5 * ( 1.0 + sqrt(5.0) );
	C0  = 299792458;
	U0  = 0.0000004*PI;
	E0  = 1.0 / ( U0*C0*C0 );

	NSTVI = 120;  NSTF  = 60;
	NSTVO = 9000; NSTIO = 0.030;
	PRIWG = 12;   PRIN  = 10;
	SECWG = 26;   SECD  = 0.07;
	SECH  = 0.30; TOPD  = 0.15;

	PRIWD = AWG( PRIWG );
	SECWD = AWG( SECWG );

	SECN  = SECH / SECWD;
	SECLN = SECN*PI*(SECD+SECWD);

	NSTTR = NSTVO / NSTVI;
	NSTVA = NSTVO * NSTIO;
	NSTII = NSTVA / NSTVI;
	NSTPF = NSTVA / ( 2.0*PI*NSTF*NSTVI*NSTVI );

	ARCLN = 0.04318*sqrt( NSTVA );

	NSTZ  = NSTVO / NSTIO;
	NSTC  = 1.0 / ( 2.0*PI*NSTF*NSTZ );
	LTRC  = NSTC * PHI;

	SECF  = 0.25 * C0 / SECLN;
	NSTCR = 1.0 / ( 2.0*PI*SECF*NSTC );
	NSTLR = NSTCR;
	PRIL  = NSTLR / ( 2.0*PI*SECF );

	TOPC  = 0.5 * TOPD / 9000000000;

	PRIF  = 1 / ( 2.0*PI*sqrt(PRIL*NSTC) );

	//PRIL = (a*a + n*n) / (8*a + 11*w);
	//PRIL = 1.17*U0*PRIN*PRIN*(PRIDO + PRIDI) / (1.0 + 2.75*(DO - DI)/(DO + DI));

	divider("\n","","=","\n\n");
	divider("","TeslaStats v1.00"," ","\n");
	divider("","Jay Phillips"," ","\n");
	divider("\n","Neon Sign Transformer","=","\n\n");
	printf("  Input:            %5.0f V  %5.3f A  %2.0f Hz\n", NSTVI, NSTII, NSTF);
	printf("  Output:           %5.0f V  %5.3f A\n", NSTVO, NSTIO);
	printf("  Power:            %5.0f W\n", NSTVA);
	printf("  Coil Ratio:       %3.0f:1\n", NSTTR);
	printf("  PFC Capacitance:  %5.1f uF\n", NSTPF*1000000);

	divider("\n","Multiple Mini Capacitor Bank","=","\n\n");
	printf("  Impedance:        %5.1f kohm\n", NSTZ/1000);
	printf("  Res Capacitance:  %5.1f nF\n", NSTC*1000000000);
	printf("  LTR Capacitance:  %5.1f nF\n", LTRC*1000000000);
	printf("  C Reactance:      %5.1f ohm\n", NSTCR );
	printf("  L Reactance:      %5.1f ohm\n", NSTLR );

	divider("\n","Primary Coil","=","\n\n");
	printf("  Inductance:      %5.1f uH\n", PRIL*1000000);
	printf("  Wire Diameter:   %5.3f mm\n", PRIWD*1000);
	printf("  Wire Length:     %5.1f m\n", PRILN);
	printf("  Wire Turns:      %5.0f turns\n", PRIN);
	printf("  Frequency:       %5.1f kHz\n", PRIF/1000);

	divider("\n","Secondary Coil","=","\n\n");
	printf("  Inductance:      %5.1f uH\n", PRIL*1000000);
	printf("  Wire Diameter:   %5.3f mm\n", SECWD*1000);
	printf("  Wire Length:     %5.1f m\n", SECLN);
	printf("  Wire Turns:      %5.0f turns\n", SECN);
	printf("  Frequency:       %5.1f kHz\n", SECF/1000);

	divider("\n","Spherical Top Load","=","\n\n");
	printf("  Diameter:        %5.1f cm\n", TOPD*100);
	printf("  Capacitance:     %5.1f pF\n", TOPC*1000000000000);

	divider("\n","Miscellaneous","=","\n\n");
	printf("  Arc Length (max):  %5.0f cm\n", ARCLN*100);

	divider("\n","","=","\n\n");

	return 0;

}

// Returns wire diameter in meters from AWG value.
float AWG( int gauge )
{

	return 0.000127 * pow( 92.0, (36.0-gauge)/39.0 );

}

void divider( char* begin, char* string, char* pad, char* end )
{

	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int i, j = 0.5 * ( w.ws_col - strlen(string) - 2 );

	printf("%s", begin);
	for ( i = 0; i < j; i++ ) printf("%s",pad);
	if ( string != "" ) printf(" %s ", string);
	else printf("%s%s", pad, pad);
	j += 2 + strlen(string);
	for ( i = j; i < w.ws_col; i++ ) printf("%s",pad);
	printf("%s", end);

}
