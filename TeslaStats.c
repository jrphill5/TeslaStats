#define AUTHOR  "Jay Phillips"
#define NAME    "TeslaStats"
#define VERSION "1.14"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>
#include "center.c"
#include "SI.c"

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

int main()
{

	// Determine whether program should run in dev mode.
	const int devel = 1;

	// Holds information about program.
	char name[strlen(NAME)+strlen(VERSION)+2];

	// Holds information about terminal size.
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	// Variables pertaining to the neon sign transformer (NST).
	//  - NSTVI:  RMS input voltage of NST expressed in volts.
	//  - NSTVIP: Peak input voltage of NST expressed in volts.
	//  - NSTII:  RMS input current of NST expressed in amps.
	//  - NSTIIP: Peak input current of NST expressed in amps.
	//  - NSTVO:  RMS output voltage of NST expressed in volts.
	//  - NSTVOP: Peak output voltage of NST expressed in volts.
	//  - NSTIO:  RMS output current of NST expressed in amps.
	//  - NSTIOP: Peak output current of NST expressed in amps.
	//  - NSTF:   Input frequency of NST expressed in hertz.
	//  - NSTVA:  Power draw of NST expressed in volt-amps.
	//  - NSTTR:  Transformer coil turn ratio of NST.
	//  - NSTZ:   Impedance of NST expressed in ohms.
	//  - NSTR:   Total resistive reactance of NST expressed in ohms.
	//  - NSTRP:  Resistance of the NST primary expressed in ohms.
	//  - NSTRS:  Resistance of the NST secondary expressed in ohms.
	//  - NSTPF:  Power factor correction capacitance for NST expressed in farads.
	float NSTVI, NSTVIP, NSTII, NSTIIP;
	float NSTVO, NSTVOP, NSTIO, NSTIOP;
	float NSTF, NSTVA, NSTTR, NSTZ, NSTR, NSTRP, NSTRS, NSTPF;

	// Variables pertaining to the primary tank capacitor (PTC).
	//  - PTCC:  Resonant capacitance for PTC expressed in farads.
	//  - LTRCS: Larger than resonant capacitance for PTC with static spark gap expressed in farads.
	//  - LTRCR: Larger than resonant capacitance for PTC with rotary spark gap expressed in farads.
	//  - PTCCR: Capacitive reactance of PTC expressed in ohms.
	//  - PRILR: Inductive reactance of PRI expressed in ohms.
	float PTCCR, PTCC, LTRCS, LTRCR, PRILR;

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
	U0  = 4.0e-7 * PI;
	E0  = 1.0 / ( U0*C0*C0 );

	// Define default values for the parameters of the Tesla coil.
	NSTVI = 120;  NSTF  = 60;
	NSTVO = 9000; NSTIO = 0.030;
	NSTRP = 1.7;  NSTRS = 13000;
	PRIWG = 12;   PRIN  = 10;
	PRIDI = 0.1;  PRIDO = 0.649;
	SECWG = 26;   SECD  = 0.07;
	SECH  = 0.30; TOPD  = 0.15;

	// Prompt user to input parameters of coil.
	if ( devel == 0 )
	{

		input( "Transformer Input Voltage       ", &NSTVI, "V"   );
		input( "Transformer Input Frequency     ", &NSTF,  "Hz"  );
		input( "Transformer Output Voltage      ", &NSTVO, "V"   );
		input( "Transformer Output Current      ", &NSTIO, "A"   );
		input( "Transformer Primary Resistance  ", &NSTRP, "ohm" );
		input( "Transformer Secondary Resistance", &NSTRS, "ohm" );
		input( "Primary Coil Wire Gauge         ", &PRIWG, "AWG" );
		input( "Primary Coil Wire Turns         ", &PRIN,  ""    );
		input( "Primary Coil Inner Diameter     ", &PRIDI, "m"   );
		input( "Primary Coil Outer Diameter     ", &PRIDO, "m"   );
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
	/* WHY?  */ LTRCR = PTCC * ( PHI + 1.0 );              // LTR Rotary Capacitance

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
	/* VERIFY */ PRILN = 0.5*PI*PRIN*(PRIDI+PRIDO);        // Primary Coil Length

	SECHD = SECH / ( SECD + SECWD );                       // Aspect Ratio

	/* VERIFY */ ARCLN = 0.04318*sqrt( NSTVA );            // Maximum Theroetical Arclength

	center("\n","",w.ws_col,'=',"\n\n");
	sprintf(name,"%s v%s",NAME,VERSION);
	center("",name,w.ws_col,' ',"\n");
	center("",AUTHOR,w.ws_col,' ',"\n");

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
	output("Wire Length:     ", &PRILN, "m"  );
	output("Wire Turns:      ", &PRIN,  ""   );
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
	printf("%s [%s]: ", description, defaultString);
	fgets(enteredString, sizeof enteredString, stdin);
	if (strlen(enteredString) != 0) sprintf(defaultString,"%s%c%s",enteredString,SIprefix(*value),unit);
	sscanf(defaultString,"%f%c%c\n",*&value,&dum,&dum);

}

void output( char* description, float* value, char* unit )
{

	printf("  %s %6.2f%c%s\n", description, *value*SIfactor(*value), SIprefix(*value), unit);

}
