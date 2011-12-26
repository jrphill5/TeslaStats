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
