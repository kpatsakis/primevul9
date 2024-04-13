double64_le_read (const unsigned char *cptr)
{	int		exponent, negative, upper, lower ;
	double	dvalue ;

	negative = (cptr [7] & 0x80) ? 1 : 0 ;
	exponent = ((cptr [7] & 0x7F) << 4) | ((cptr [6] >> 4) & 0xF) ;

	/* Might not have a 64 bit long, so load the mantissa into a double. */
	upper = ((cptr [6] & 0xF) << 24) | (cptr [5] << 16) | (cptr [4] << 8) | cptr [3] ;
	lower = (cptr [2] << 16) | (cptr [1] << 8) | cptr [0] ;

	if (exponent == 0 && upper == 0 && lower == 0)
		return 0.0 ;

	dvalue = upper + lower / ((double) 0x1000000) ;
	dvalue += 0x10000000 ;

	exponent = exponent - 0x3FF ;

	dvalue = dvalue / ((double) 0x10000000) ;

	if (negative)
		dvalue *= -1 ;

	if (exponent > 0)
		dvalue *= pow (2.0, exponent) ;
	else if (exponent < 0)
		dvalue /= pow (2.0, abs (exponent)) ;

	return dvalue ;
} /* double64_le_read */