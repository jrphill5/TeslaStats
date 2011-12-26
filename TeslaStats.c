#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>

// Global constants and ratios.
//  - PI:  Ratio of circumference to diameter of a circle.
//  - PHI: Golden ratio. 
//  - C0:  Speed of light in free space.
//  - U0:  Magnetic permeability of free space.
//  - E0:  Electric permittivity of free space.
double PI, PHI, C0, U0, E0;

// Converts American wire gauge (WG) to wire diameter (WD) expressed in meters.
float WD( float WG );
// Converts wire diameter (WD) expressed in meters to American wire gauge (WG).
float WG( float WD );

// Returns the empirical self-capacitance of a helical coil with radius R and length L.
float medhurst( float R, float L );
double SIfactor( double value );
char SIprefix( double value );

// Formats and prints a full terminal width divider.
//  - begin: The string to be printed at beginning of divider.
//  - text:  The text that will name the divider.
//  - pad:   The padding character that makes up divider.
//  - end:   The string to be printed at end of divider.
void divider( char* begin, char* text, char* pad, char* end );

int main()
{

	// Variables pertaining to the neon sign transformer (NST).
	//  - NSTVI: Input voltage of NST expressed in volts.
	//  - NSTII: Input current of NST expressed in amps.
	//  - NSTF:  Input frequency of NST expressed in hertz.
	//  - NSTVO: Output voltage of NST expressed in volts.
	//  - NSTIO: Output current of NST expressed in amps.
	//  - NSTVA: Power draw of NST expressed in volt-amps.
	//  - NSTTR: Transformer coil turn ratio of NST.
	//  - NSTZ:  Impedance of NST expressed in ohms.
	//  - NSTPF: Power factor correction capacitance for NST expressed in farads.
	float NSTVI, NSTII, NSTF, NSTVO, NSTIO, NSTVA, NSTTR, NSTZ, NSTPF;

	// Variables pertaining to the multiple mini capacitor bank (MMC).
	//  - MMCC:  Resonant capacitance for MMC expressed in farads.
	//  - LTRC:  Larger than resonant capacitance for MMC expressed in farads.
	//  - MMCCR: Capacitive reactance of MMC expressed in ohms.
	//  - PRILR: Inductive reactance of PRI expressed in ohms.
	float MMCCR, MMCC, LTRC, PRILR;

	// Variables pertaining to the primary coil (PRI).
	//  - PRIWG: American wire gauge of PRI.
	//  - PRIWD: Wire diameter of PRI expressed in meters.
	//  - PRIDI: Inner diameter of PRI expressed in meters.
	//  - PRIDO: Outer diameter of PRI expressed in meters.
	//  - PRIF:  Resonant frequency of PRI expressed in Hertz.
	//  - PRILN: Length of PRI expressed in meters.
	//  - PRIL:  Inductance of PRI expressed in Henries.
	//  - PRIN:  Number of turns of wire in PRI.
	float PRIWG, PRIWD, PRIDI, PRIDO, PRIF, PRILN, PRIL, PRIN;

	// Variables pertaining to the secondary coil (SEC).
	//  - SECWG: American wire gauge of SEC.
	//  - SECWD: Wire diameter of SEC expressed in meters.
	//  - SECD:  Diameter of SEC expressed in meters.
	//  - SECH:  Height of SEC expressed in meters.
	//  - SECF:  Resonant frequency of SEC expressed in hertz.
	//  - SECLN: Length of SEC expressed in meters.
	//  - SECL:  Inductance of SEC expressed in henries.
	//  - SECC:  Capacitance of SEC expressed in farads.
	//  - SECN:  Number of turns of wire in SEC.
	//  - SECHD: Coil height to diameter ratio.
	float SECWG, SECWD, SECD, SECH, SECF, SECLN, SECL, SECC, SECN, SECHD;

	// Variables pertaining to the top load (TOP).
	//  - TOPD:  Diameter of TOP expressed in meters.
	//  - TOPC:  Capacitance of TOP expressed in Farads.
	float TOPD, TOPC;

	// Miscellaneous variables.
	//  - ARCLN: Maximum length of arc expressed in meters.
	float ARCLN;

	// Define values for the constants and ratios declared above.
	PI  = 3.1415926535897932384626433832795;
	PHI = 0.5 * ( 1.0 + sqrt(5.0) );
	C0  = 299792458;
	U0  = 0.0000004*PI;
	E0  = 1.0 / ( U0*C0*C0 );

	// Define values for the parameters of the Tesla coil.
	NSTVI = 120;  NSTF  = 60;
	NSTVO = 9000; NSTIO = 0.030;
	PRIWG = 12;   PRIN  = 10;
	PRIDI = 0.1;  PRIDO = 0.649;
	SECWG = 26;   SECD  = 0.07;
	SECH  = 0.30; TOPD  = 0.15;

	/* GOOD */ PRIWD = WD( PRIWG );
	/* GOOD */ SECWD = WD( SECWG );

	// Take into account the wire insulation. (10%?)
	/* GOOD */ SECN  = SECH / SECWD;
	/* GOOD */ SECLN = SECN*PI*(SECD+SECWD);
	SECL  = ( 0.25*SECN*SECN*(SECD+SECWD)*(SECD+SECWD) / (4.5*(SECD+SECWD)+10.0*SECH) / 25.4 ) / 1000.0;

	/* GOOD */ NSTTR = NSTVO / NSTVI;
	/* GOOD */ NSTVA = NSTVO * NSTIO;
	/* GOOD */ NSTII = NSTVA / NSTVI;
	/* GOOD */ NSTPF = NSTVA / ( 2.0*PI*NSTF*NSTVI*NSTVI );

	/* GOOD */ NSTZ  = NSTVO / NSTIO;
	/* GOOD */ MMCC  = 1.0 / ( 2.0*PI*NSTF*NSTZ );
	/* GOOD */ LTRC  = MMCC * PHI;

	SECF  = 0.25 * C0 / SECLN;
	MMCCR = 1.0 / ( 2.0*PI*SECF*MMCC );
	PRILR = MMCCR;
	PRIL  = PRILR / ( 2.0*PI*SECF );

	TOPC  = 0.5 * TOPD / 9000000000;

	PRIF  = 1.0 / ( 2.0*PI*sqrt(PRIL*MMCC) );
	SECF  = PRIF;
	PRILN = 0.5*PI*PRIN*(PRIDI+PRIDO);

	ARCLN = 0.04318*sqrt( NSTVA );
	SECHD = SECH / ( SECD + SECWD );
	SECC = medhurst(0.5*SECD, SECH);

	divider("\n","","=","\n\n");
	divider("","TeslaStats v1.03"," ","\n");
	divider("","Jay Phillips"," ","\n");
	divider("\n","Neon Sign Transformer","=","\n\n");
	printf("  Input:            %6.2f%cV  %6.2f%cA  %6.2f%cHz\n", NSTVI*SIfactor(NSTVI),SIprefix(NSTVI),NSTII*SIfactor(NSTII),SIprefix(NSTII),NSTF*SIfactor(NSTF),SIprefix(NSTF));
	printf("  Output:           %6.2f%cV  %6.2f%cA\n",            NSTVO*SIfactor(NSTVO),SIprefix(NSTVO),NSTIO*SIfactor(NSTIO),SIprefix(NSTIO));
	printf("  Power:            %6.0f%cW\n",    NSTVA* SIfactor(NSTVA), SIprefix(NSTVA));
	printf("  Step-up Ratio:    %6.1f:1\n",     NSTTR);
	printf("  PFC Capacitance:  %6.1f%cF\n",    NSTPF* SIfactor(NSTPF), SIprefix(NSTPF));
	printf("  Impedance:        %6.1f%cohm\n",  NSTZ*  SIfactor(NSTZ),  SIprefix(NSTZ));

	divider("\n","Multiple Mini Capacitor Bank","=","\n\n");
	printf("  C Reactance:      %6.1f%cohm\n",  MMCCR* SIfactor(MMCCR), SIprefix(MMCCR));
	printf("  Res Capacitance:  %6.1f%cF\n",    MMCC*  SIfactor(MMCC),  SIprefix(MMCC));
	printf("  LTR Capacitance:  %6.1f%cF\n",    LTRC*  SIfactor(LTRC),  SIprefix(LTRC));

	divider("\n","Primary Coil","=","\n\n");
	printf("  L Reactance:      %6.1f%cohm\n",  PRILR* SIfactor(PRILR), SIprefix(PRILR));
	printf("  Wire Gauge:       %6.1f AWG\n",   PRIWG);
	printf("  Wire Diameter:    %6.3f%cm\n",    PRIWD* SIfactor(PRIWD), SIprefix(PRIWD));
	printf("  Wire Length:      %6.1f%cm\n",    PRILN* SIfactor(PRILN), SIprefix(PRILN));
	printf("  Wire Turns:       %6.0f turns\n", PRIN);
	printf("  Inductance:       %6.1f%cH\n",    PRIL*  SIfactor(PRIL),  SIprefix(PRIL));
	printf("  Frequency:        %6.1f%cHz\n",   PRIF*  SIfactor(PRIF),  SIprefix(PRIF));

	divider("\n","Secondary Coil","=","\n\n");
	printf("  Form Diameter:    %6.1f%cm\n",    SECD*  SIfactor(SECD),  SIprefix(SECD));
	printf("  Form Height:      %6.1f%cm\n",    SECH*  SIfactor(SECH),  SIprefix(SECH));
	printf("  Aspect Ratio:     %6.1f:1\n",     SECHD);
	printf("  Wire Gauge:       %6.1f AWG\n",   SECWG);
	printf("  Wire Diameter:    %6.1f%cm\n",    SECWD* SIfactor(SECWD), SIprefix(SECWD));
	printf("  Wire Length:      %6.1f%cm\n",    SECLN* SIfactor(SECLN), SIprefix(SECLN));
	printf("  Wire Turns:       %6.0f turns\n", SECN);
	printf("  Inductance:       %6.1f%cH\n",    SECL*  SIfactor(SECL),  SIprefix(SECL));
	printf("  Capacitance:      %6.1f%cF\n",    SECC*  SIfactor(SECC),  SIprefix(SECC));
	printf("  Frequency:        %6.1f%cHz\n",   SECF*  SIfactor(SECF),  SIprefix(SECF));

	divider("\n","Spherical Top Load","=","\n\n");
	printf("  Diameter:         %6.1f%cm\n",    TOPD*  SIfactor(TOPD),  SIprefix(TOPD));
	printf("  Capacitance:      %6.1f%cF\n",    TOPC*  SIfactor(TOPC),  SIprefix(TOPC));

	divider("\n","Miscellaneous","=","\n\n");
	printf("  Arc Length (max): %6.1f%cm\n",    ARCLN* SIfactor(ARCLN), SIprefix(ARCLN));

	divider("\n","","=","\n\n");

	return 0;

}

float WD( float WG ) { return 0.000127 * pow( 92.0, ( 36.0 - WG ) / 39.0 ); }
float WG( float WD ) { return -39.0 * log10( WD / 0.000127 ) / log10( 92.0 ) + 36.0; }

void divider( char* begin, char* text, char* pad, char* end )
{

	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int i, j = 0.5 * ( w.ws_col - strlen(text) - 2 );

	printf("%s", begin);
	for ( i = 0; i < j; i++ ) printf("%s",pad);
	if ( *text != '\0' ) printf(" %s ", text);
	else printf("%s%s", pad, pad);
	j += 2 + strlen(text);
	for ( i = j; i < w.ws_col; i++ ) printf("%s",pad);
	printf("%s", end);

}

float medhurst( float R, float L )
{

	return ( 1/0.0254 * (0.29*L + R * ( 0.41 + 1.94*sqrt(R / L) ) ) ) / 1000000000000.0;

}

// allow specification of preferred unit
double SIfactor( double value )
{

	if ( value < 1.0 )
	{

		if ( ( value *= 1000.0 ) > 1.0 ) return 1.0e3;   // milli
		if ( ( value *= 1000.0 ) > 1.0 ) return 1.0e6;   // micro
		if ( ( value *= 1000.0 ) > 1.0 ) return 1.0e9;   // nano
		if ( ( value *= 1000.0 ) > 1.0 ) return 1.0e12;  // pico

	}
	else
	{

		if ( ( value /= 1000.0 ) < 1.0 ) return 1.0;     // base
		if ( ( value /= 1000.0 ) < 1.0 ) return 1.0e-3;  // kilo
		if ( ( value /= 1000.0 ) < 1.0 ) return 1.0e-6;  // mega
		if ( ( value /= 1000.0 ) < 1.0 ) return 1.0e-9;  // giga
		if ( ( value /= 1000.0 ) < 1.0 ) return 1.0e-12; // tera

	}

	return 1.0;

}

char SIprefix( double value )
{

	if ( value < 1.0 )
	{

		if ( ( value *= 1000.0 ) > 1.0 ) return 'm'; // milli
		if ( ( value *= 1000.0 ) > 1.0 ) return 'u'; // micro
		if ( ( value *= 1000.0 ) > 1.0 ) return 'n'; // nano
		if ( ( value *= 1000.0 ) > 1.0 ) return 'p'; // pico

	}
	else
	{

		if ( ( value /= 1000.0 ) < 1.0 ) return ' '; // base
		if ( ( value /= 1000.0 ) < 1.0 ) return 'K'; // kilo
		if ( ( value /= 1000.0 ) < 1.0 ) return 'M'; // mega
		if ( ( value /= 1000.0 ) < 1.0 ) return 'G'; // giga
		if ( ( value /= 1000.0 ) < 1.0 ) return 'T'; // tera

	}

	return ' ';

}
