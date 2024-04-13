double64_get_capability	(SF_PRIVATE *psf)
{	union
	{	double			d ;
		unsigned char	c [8] ;
	} data ;

	data.d = 1.234567890123456789 ; /* Some abitrary value. */

	if (! psf->ieee_replace)
	{	/* If this test is true ints and floats are compatible and little endian. */
		if (data.c [0] == 0xfb && data.c [1] == 0x59 && data.c [2] == 0x8c && data.c [3] == 0x42 &&
			data.c [4] == 0xca && data.c [5] == 0xc0 && data.c [6] == 0xf3 && data.c [7] == 0x3f)
			return DOUBLE_CAN_RW_LE ;

		/* If this test is true ints and floats are compatible and big endian. */
		if (data.c [0] == 0x3f && data.c [1] == 0xf3 && data.c [2] == 0xc0 && data.c [3] == 0xca &&
			data.c [4] == 0x42 && data.c [5] == 0x8c && data.c [6] == 0x59 && data.c [7] == 0xfb)
			return DOUBLE_CAN_RW_BE ;
		} ;

	/* Doubles are broken. Don't expect reading or writing to be fast. */
	psf_log_printf (psf, "Using IEEE replacement code for double.\n") ;

	return (CPU_IS_LITTLE_ENDIAN) ? DOUBLE_BROKEN_LE : DOUBLE_BROKEN_BE ;
} /* double64_get_capability */