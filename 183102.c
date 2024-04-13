double64_be_write (double in, unsigned char *out)
{	int		exponent, mantissa ;

	memset (out, 0, sizeof (double)) ;

	if (fabs (in) < 1e-30)
		return ;

	if (in < 0.0)
	{	in *= -1.0 ;
		out [0] |= 0x80 ;
		} ;

	in = frexp (in, &exponent) ;

	exponent += 1022 ;

	out [0] |= (exponent >> 4) & 0x7F ;
	out [1] |= (exponent << 4) & 0xF0 ;

	in *= 0x20000000 ;
	mantissa = lrint (floor (in)) ;

	out [1] |= (mantissa >> 24) & 0xF ;
	out [2] = (mantissa >> 16) & 0xFF ;
	out [3] = (mantissa >> 8) & 0xFF ;
	out [4] = mantissa & 0xFF ;

	in = fmod (in, 1.0) ;
	in *= 0x1000000 ;
	mantissa = lrint (floor (in)) ;

	out [5] = (mantissa >> 16) & 0xFF ;
	out [6] = (mantissa >> 8) & 0xFF ;
	out [7] = mantissa & 0xFF ;

	return ;
} /* double64_be_write */