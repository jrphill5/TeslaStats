#define AUTHOR  "Jay Phillips"
#define NAME    "TeslaStats"
#define VERSION "1.16"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>
#include "Shared.c"

// Global constants and ratios.
//  - PI:  Ratio of circumference to diameter of a circle.
//  - PHI: Golden ratio. 
//  - C0:  Speed of light in free space.
//  - U0:  Magnetic permeability of free space.
//  - E0:  Electric permittivity of free space.
double PI, PHI, C0, U0, E0;

// Convert between American wire gauge (WG) and wire diameter (WD) expressed in meters.
float WD( float WG );
float WG( float WD );

// Returns the empirical self-capacitance of a helical coil with radius R and length L.
float medhurst( float R, float L );

// Return the SI unit autoscale factor and prefix of a given value.
extern double SIfactor( double value );
extern char SIprefix( double value );

// Formats and centers text.
extern void center( char* begin, char* text, int col, char pad, char* end );

// Control the input and output of parameter values.
void input(  char* description, float* value, char* unit );
void output( char* description, float* value, char* unit );

// Parse settings from external file.
int parseSettings( char* file );
int parseParameter( char* param, float value );

// Write settings to external file.
int writeSettings( char* file );

// Input parameters pertaining to the neon sign transformer (NST).
//  - NSTVI:  RMS input voltage of NST expressed in volts.
//  - NSTF:   Input frequency of NST expressed in hertz.
//  - NSTVO:  RMS output voltage of NST expressed in volts.
//  - NSTIO:  RMS output current of NST expressed in amps.
//  - NSTRP:  Resistance of the NST primary expressed in ohms.
//  - NSTRS:  Resistance of the NST secondary expressed in ohms.
float NSTVI, NSTF, NSTVO, NSTIO, NSTRP, NSTRS;

// Output parameters pertaining to the neon sign transformer (NST).
//  - NSTVIP: Peak input voltage of NST expressed in volts.
//  - NSTII:  RMS input current of NST expressed in amps.
//  - NSTIIP: Peak input current of NST expressed in amps.
//  - NSTVOP: Peak output voltage of NST expressed in volts.
//  - NSTIOP: Peak output current of NST expressed in amps.
//  - NSTVA:  Power draw of NST expressed in volt-amps.
//  - NSTTR:  Transformer coil turn ratio of NST.
//  - NSTZ:   Impedance of NST expressed in ohms.
//  - NSTR:   Total resistive reactance of NST expressed in ohms.
//  - NSTPF:  Power factor correction capacitance for NST expressed in farads.
float NSTVIP, NSTII, NSTIIP, NSTVOP, NSTIOP, NSTVA, NSTTR, NSTZ, NSTR, NSTPF;

// Output parameters pertaining to the primary tank capacitor (PTC).
//  - PTCC:  Resonant capacitance for PTC expressed in farads.
//  - LTRCS: Larger than resonant capacitance for PTC with static spark gap expressed in farads.
//  - LTRCR: Larger than resonant capacitance for PTC with rotary spark gap expressed in farads.
//  - PTCCR: Capacitive reactance of PTC expressed in ohms.
float PTCCR, PTCC, LTRCS, LTRCR;

// Input parameters pertaining to the primary coil (PRI).
//  - PRIWG: American wire gauge of PRI.
//  - PRIN:  Number of turns of wire in PRI.
//  - PRIRI: Inner radius of PRI expressed in meters.
//  - PRIS:  Separation of PRI wrappings expressed in meters.
float PRIWG, PRIN, PRIRI, PRIS;

// Output parameters pertaining to the primary coil (PRI).
//  - PRILR: Inductive reactance of PRI expressed in ohms.
//  - PRIWD: Wire diameter of PRI expressed in meters.
//  - PRIRO: Outer radius of PRI expressed in meters.
//  - PRIF:  Resonant frequency of PRI expressed in Hertz.
//  - PRILN: Length of PRI expressed in meters.
//  - PRIL:  Inductance of PRI expressed in Henries.
float PRILR, PRIWD, PRIRO, PRIF, PRILN, PRIL;

// Input parameters pertaining to the secondary coil (SEC).
//  - SECWG: American wire gauge of SEC.
//  - SECD:  Diameter of SEC expressed in meters.
//  - SECH:  Height of SEC expressed in meters.
float SECWG, SECD, SECH;

// Output parameters pertaining to the secondary coil (SEC).
//  - SECWD: Wire diameter of SEC expressed in meters.
//  - SECF:  Resonant frequency of SEC expressed in hertz.
//  - SECLN: Length of SEC expressed in meters.
//  - SECL:  Inductance of SEC expressed in henries.
//  - SECC:  Capacitance of SEC expressed in farads.
//  - SECN:  Number of turns of wire in SEC.
//  - SECHD: Coil height to diameter ratio.
float SECWD, SECF, SECLN, SECL, SECC, SECN, SECHD;

// Input parameters pertaining to the top load (TOP).
//  - TOPD:  Diameter of TOP expressed in meters.
float TOPD;

// Output parameters pertaining to the top load (TOP).
//  - TOPC:  Capacitance of TOP expressed in Farads.
float TOPC;

// Miscellaneous output parameters.
//  - ARCLN: Maximum length of arc expressed in meters.
float ARCLN;

int main()
{

	// Determine whether program should run in dev mode.
	const int devel = 0;

	// Holds information about program.
	char name[strlen(NAME)+strlen(VERSION)+2];

	// Holds information about terminal size.
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	// Define values for the constants and ratios declared above.
	PI  = 3.1415926535897932384626433832795;
	PHI = 0.5 * ( 1.0 + sqrt(5.0) );
	C0  = 299792458;
	U0  = 4.0e-7 * PI;
	E0  = 1.0 / ( U0*C0*C0 );

	center("\n","",w.ws_col,'=',"\n\n");
	sprintf(name,"%s v%s",NAME,VERSION);
	center("",name,w.ws_col,' ',"\n");
	center("",AUTHOR,w.ws_col,' ',"\n");

	// Read in parameters from external file.
	center("\n","Parse Parameters",w.ws_col,'=',"\n\n");
	parseSettings("parameters.dat");

	// Prompt user to input parameters of coil.
	if ( devel == 0 )
	{

		center("\n","Edit Parameters",w.ws_col,'=',"\n\n");
		input( "Transformer Input Voltage       ", &NSTVI, "V"   );
		input( "Transformer Input Frequency     ", &NSTF,  "Hz"  );
		input( "Transformer Output Voltage      ", &NSTVO, "V"   );
		input( "Transformer Output Current      ", &NSTIO, "A"   );
		input( "Transformer Primary Resistance  ", &NSTRP, "ohm" );
		input( "Transformer Secondary Resistance", &NSTRS, "ohm" );
		input( "Primary Coil Wire Gauge         ", &PRIWG, "AWG" );
		input( "Primary Coil Wire Turns         ", &PRIN,  ""    );
		input( "Primary Coil Inner Radius       ", &PRIRI, "m"   );
		input( "Primary Coil Separation         ", &PRIS , "m"   );
		input( "Secondary Coil Wire Gauge       ", &SECWG, "AWG" );
		input( "Secondary Coil Form Diameter    ", &SECD,  "m"   );
		input( "Secondary Coil Form Height      ", &SECH,  "m"   );
		input( "Topload Sphere Diameter         ", &TOPD,  "m"   );

	}

	// Calculate primary wire diameter from AWG value.
	PRIWD = WD( PRIWG );
	// Calculate secondary wire diameter from AWG value accounting for single insulation.
	// Single insulation: 0.0014in 3.55600e-5m
	// Double insulation: 0.0026in 6.60400e-5m
	SECWD = WD( SECWG ) + 3.55600e-5;

	// Calculate outer diameter of PRI.
	PRIRO = PRIRI + PRIN * PRIS;               // Primary Outer Radius

	SECN  = SECH / SECWD;                      // Secondary Wrap Number
	SECLN = SECN*PI*(SECD+SECWD);              // Secondary Wire Length

	// Calculate inductance of secondary using Wheeler's empirical formula.
	// Accurate to within 1% for SECL > 0.4*SECD. 
	SECL  = SECN*SECN*(SECD+SECWD)*(SECD+SECWD) / ( 2.54e4 * (18.0*(SECD+SECWD)+40.0*SECH) );

	// Calculate inductance of secondary using theoretical solenoid equation.
	// Implement Nagaoka coefficient to increase accuracy.
	//SECL  = 0.25*PI*U0*SECN*SECN*(SECD+SECWD)*(SECD+SECWD)/SECH;

	// Calculate transformer power performance statistics.
	NSTTR = NSTVO / NSTVI;
	NSTVA = NSTVO * NSTIO;
	NSTII = NSTVA / NSTVI;
	NSTPF = NSTVA / ( 2.0*PI*NSTF*NSTVI*NSTVI );

	// Calculate peak voltages and currents from RMS values.
	NSTVIP = NSTVI * sqrt(2.0);
	NSTIIP = NSTII * sqrt(2.0);
	NSTVOP = NSTVO * sqrt(2.0);
	NSTIOP = NSTIO * sqrt(2.0);

	NSTZ  = NSTVO / NSTIO;                                 // Impedance
	NSTR  = NSTRS + NSTRP * NSTTR * NSTTR;                 // Reactance
	/* SIGN? */ NSTZ  = sqrt( NSTZ * NSTZ - NSTR * NSTR ); // Total Impedance
	PTCC  = 1.0 / ( 2.0*PI*NSTF*NSTZ );                    // Resonant Capacitance
	LTRCS = PTCC * PHI;                                    // LTR Static Capacitance
	LTRCR = PTCC * PHI * PHI;                              // LTR Rotary Capacitance

	// Calculate resonant frequency of the system.
	SECC = medhurst(0.5*SECD, SECH);                       // Secondary Self-Capacitance
	TOPC  = 2.0 * PI * E0 * TOPD;                          // Topload Capacitance
	// Estimate the secondary resonant frequency.
	/* VERIFY */ SECF  = 0.25 * C0 / SECLN;
	// Calculate the resonant frequency of the secondary circuit.
	SECF  = 1.0 / ( 2.0 * PI * sqrt( SECL * ( SECC + TOPC ) ) );
	PRIF  = SECF;

	// Calculate primary inductance such that capacitive and inductive reactances cancel.
	// Assume use of LTR static capacitor bank.
	PTCCR = 1.0 / ( 2.0*PI*PRIF*LTRCS );                   // Capacitive Reactance
	PRILR = PTCCR;                                         // Inductive Reactance
	PRIL  = PRILR / ( 2.0*PI*PRIF );                       // Primary Inductance
	//PRIF  = 1.0 / ( 2.0*PI*sqrt(PRIL*LTRCS) );           // Primary Resonant Frequency
	//PRILN = 0.5*PI*PRIN*(PRIDI+PRIDO);                   // Primary Coil Length via DI and DO
    PRILN = PI * PRIN * PRIS * ( PRIN + 2.0*PI );          // Primary Coil Length via RI and dR

	SECHD = SECH / ( SECD + SECWD );                       // Aspect Ratio

	/* VERIFY */ ARCLN = 0.04318*sqrt( NSTVA );            // Maximum Theroetical Arclength

	center("\n","Neon Sign Transformer",w.ws_col,'=',"\n\n");
	printf("  Input (RMS):      %6.2f%cV %6.2f%cA %6.2f%cHz\n",
		NSTVI*  SIfactor(NSTVI),  SIprefix(NSTVI),
		NSTII*  SIfactor(NSTII),  SIprefix(NSTII),
		NSTF*   SIfactor(NSTF),   SIprefix(NSTF));
	printf("  Input (Peak):     %6.2f%cV %6.2f%cA %6.2f%cHz\n",
		NSTVIP* SIfactor(NSTVIP), SIprefix(NSTVIP),
		NSTIIP* SIfactor(NSTIIP), SIprefix(NSTIIP),
		NSTF*   SIfactor(NSTF),   SIprefix(NSTF));
	printf("  Output (RMS):     %6.2f%cV %6.2f%cA %6.2f%cHz\n",
		NSTVO*  SIfactor(NSTVO),  SIprefix(NSTVO),
		NSTIO*  SIfactor(NSTIO),  SIprefix(NSTIO),
		NSTF*   SIfactor(NSTF),   SIprefix(NSTF));
	printf("  Output (Peak):    %6.2f%cV %6.2f%cA %6.2f%cHz\n",
		NSTVOP* SIfactor(NSTVOP), SIprefix(NSTVOP),
		NSTIOP* SIfactor(NSTIOP), SIprefix(NSTIOP),
		NSTF*   SIfactor(NSTF),   SIprefix(NSTF));
	output("Step-up Ratio:   ", &NSTTR, ""   );
	output("Power:           ", &NSTVA, "VA" );
	output("PFC Capacitance: ", &NSTPF, "F"  );
	output("Impedance:       ", &NSTZ,  "ohm");

	center("\n","Primary Tank Capacitor",w.ws_col,'=',"\n\n");
	output("Cap Reactance:   ", &PTCCR, "ohm");
	output("Res Capacitance: ", &PTCC,  "F"  );
	output("LTR Static Cap:  ", &LTRCS, "F"  );
	output("LTR Rotary Cap:  ", &LTRCR, "F"  );

	center("\n","Primary Coil",w.ws_col,'=',"\n\n");
	output("L Reactance:     ", &PRILR, "ohm");
	output("Wire Gauge:      ", &PRIWG, "AWG");
	output("Wire Diameter:   ", &PRIWD, "m"  );
	output("Wire Turns:      ", &PRIN,  ""   );
	output("Wire Length:     ", &PRILN, "m"  );
	output("Inner Radius:    ", &PRIRI, "m"  );
	output("Winding Sep:     ", &PRIS,  "m"  );
	output("Outer Radius:    ", &PRIRO, "m"  );
	output("Inductance:      ", &PRIL,  "H"  );
	output("Res Frequency:   ", &PRIF,  "Hz" );

	center("\n","Secondary Coil",w.ws_col,'=',"\n\n");
	output("Form Diameter:   ", &SECD,  "m"  );
	output("Form Height:     ", &SECH,  "m"  );
	output("Aspect Ratio:    ", &SECHD, ""   );
	output("Wire Gauge:      ", &SECWG, "AWG");
	output("Wire Diameter:   ", &SECWD, "m"  );
	output("Wire Length:     ", &SECLN, "m"  );
	output("Wire Turns:      ", &SECN,  ""   );
	output("Inductance:      ", &SECL,  "H"  );
	output("Capacitance:     ", &SECC,  "F"  );
	output("Res Frequency:   ", &SECF,  "Hz" );

	center("\n","Spherical Top Load",w.ws_col,'=',"\n\n");
	output("Diameter:        ", &TOPD,  "m"  );
	output("Capacitance:     ", &TOPC,  "F"  );

	center("\n","Miscellaneous",w.ws_col,'=',"\n\n");
	output("Arc Length (max):", &ARCLN, "m"  );

	// Write values to external file.
	center("\n","Write Values",w.ws_col,'=',"\n\n");
	writeSettings("parameters.out");

	center("\n","",w.ws_col,'=',"\n\n");

	return 0;

}

float WD( float WG ) { return 0.000127 * pow( 92.0, ( 36.0 - WG ) / 39.0 ); }
float WG( float WD ) { return -39.0 * log10( WD / 0.000127 ) / log10( 92.0 ) + 36.0; }

float medhurst( float R, float L )
{

	return ( 1/0.0254 * (0.29*L + R * ( 0.41 + 1.94*sqrt(R / L) ) ) ) / 1000000000000.0;

}

void input( char* description, float* value, char* unit )
{

	char dum;
	char defaultString[10];
	char enteredString[10];

	sprintf(defaultString,"%6.2f%c%-3s",*value*SIfactor(*value),SIprefix(*value),unit);
	printf("  %s [%s]: ", description, defaultString);
	fgets(enteredString, sizeof enteredString, stdin);
	if (strlen(enteredString) != 0) sprintf(defaultString,"%s%c%s",enteredString,SIprefix(*value),unit);
	sscanf(defaultString,"%f%c%c\n",*&value,&dum,&dum);

}

void output( char* description, float* value, char* unit )
{

	printf("  %s %6.2f%c%s\n", description, *value*SIfactor(*value), SIprefix(*value), unit);

}

int parseSettings( char* file )
{

	char param[6] = {0};
	float value = 0.0;
	FILE* settings = fopen( file, "r" );

	if ( !settings )
	{

		fprintf(stderr, "Cannot open %s for parsing!\n", file);
		return 1;

	}
	else
	{

		while ( fscanf(settings, "%s %f", param, &value) != EOF )
			parseParameter( param, value );
		fclose( settings );
		return 0;

	}

}

int parseParameter( char* param, float value )
{

	     if ( strcmp( param, "NSTVI" ) == 0 ) NSTVI = value;
	else if ( strcmp( param, "NSTF"  ) == 0 ) NSTF  = value;
	else if ( strcmp( param, "NSTVO" ) == 0 ) NSTVO = value;
	else if ( strcmp( param, "NSTIO" ) == 0 ) NSTIO = value;
	else if ( strcmp( param, "NSTRP" ) == 0 ) NSTRP = value;
	else if ( strcmp( param, "NSTRS" ) == 0 ) NSTRS = value;
	else if ( strcmp( param, "PRIWG" ) == 0 ) PRIWG = value;
	else if ( strcmp( param, "PRIN"  ) == 0 ) PRIN  = value;
	else if ( strcmp( param, "PRIRI" ) == 0 ) PRIRI = value;
	else if ( strcmp( param, "PRIS"  ) == 0 ) PRIS  = value;
	else if ( strcmp( param, "SECWG" ) == 0 ) SECWG = value;
	else if ( strcmp( param, "SECD"  ) == 0 ) SECD  = value;
	else if ( strcmp( param, "SECH"  ) == 0 ) SECH  = value;
	else if ( strcmp( param, "TOPD"  ) == 0 ) TOPD  = value;
	else
	{

		fprintf(stderr, "  %s not valid parameter.\n", param);
		return 1;

	}

    printf("  %-6s %0.2e\n", param, value);
	return 0;

}

int writeSettings( char* file )
{

	FILE* settings = fopen( file, "w" );

	if ( !settings )
	{

		fprintf(stderr, "Cannot open %s for writing!\n", file);
		return 1;

	}
	else
	{

		fprintf( settings, "NSTVI	%e\n", NSTVI );
		fprintf( settings, "NSTF	%e\n", NSTF  );
		fprintf( settings, "NSTVO	%e\n", NSTVO );
		fprintf( settings, "NSTIO	%e\n", NSTIO );
		fprintf( settings, "NSTRP	%e\n", NSTRP );
		fprintf( settings, "NSTRS	%e\n", NSTRS );
		fprintf( settings, "NSTVIP	%e\n", NSTVIP);
		fprintf( settings, "NSTII	%e\n", NSTII );
		fprintf( settings, "NSTIIP	%e\n", NSTIIP);
		fprintf( settings, "NSTVOP	%e\n", NSTVOP);
		fprintf( settings, "NSTIOP	%e\n", NSTIOP);
		fprintf( settings, "NSTVA	%e\n", NSTVA );
		fprintf( settings, "NSTTR	%e\n", NSTTR );
		fprintf( settings, "NSTZ	%e\n", NSTZ  );
		fprintf( settings, "NSTR	%e\n", NSTR  );
		fprintf( settings, "NSTPF	%e\n", NSTPF );

		fprintf( settings, "PTCCR	%e\n", PTCCR );
		fprintf( settings, "PTCC	%e\n", PTCC  );
		fprintf( settings, "LTRCS	%e\n", LTRCS );
		fprintf( settings, "LTRCR	%e\n", LTRCR );

		fprintf( settings, "PRIWG	%e\n", PRIWG );
		fprintf( settings, "PRIN	%e\n", PRIN  );
		fprintf( settings, "PRIRI	%e\n", PRIRI );
		fprintf( settings, "PRIS	%e\n", PRIS  );
		fprintf( settings, "PRILR	%e\n", PRILR );
		fprintf( settings, "PRIWD	%e\n", PRIWD );
		fprintf( settings, "PRIRO	%e\n", PRIRO );
		fprintf( settings, "PRIF	%e\n", PRIF  );
		fprintf( settings, "PRILN	%e\n", PRILN );
		fprintf( settings, "PRIL	%e\n", PRIL  );

		fprintf( settings, "SECWG	%e\n", SECWG );
		fprintf( settings, "SECD	%e\n", SECD  );
		fprintf( settings, "SECH	%e\n", SECH  );
		fprintf( settings, "SECWD	%e\n", SECWD );
		fprintf( settings, "SECF	%e\n", SECF  );
		fprintf( settings, "SECLN	%e\n", SECLN );
		fprintf( settings, "SECL	%e\n", SECL  );
		fprintf( settings, "SECC	%e\n", SECC  );
		fprintf( settings, "SECN	%e\n", SECN  );
		fprintf( settings, "SECHD	%e\n", SECHD );

		fprintf( settings, "TOPD	%e\n", TOPD  );
		fprintf( settings, "TOPC	%e\n", TOPC  );

		fprintf( settings, "ARCLN	%e\n", ARCLN );

		fclose( settings );

		return 0;

	}

}
