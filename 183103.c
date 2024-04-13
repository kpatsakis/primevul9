replace_read_d2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;
	double		scale ;

	bufferlen = ARRAY_LEN (ubuf.dbuf) ;
	scale = (psf->float_int_mult == 0) ? 1.0 : 0x7FFFFFFF / psf->float_max ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = psf_fread (ubuf.dbuf, sizeof (double), bufferlen, psf) ;

		if (psf->data_endswap == SF_TRUE)
			endswap_double_array (ubuf.dbuf, bufferlen) ;

		d2bd_read (ubuf.dbuf, bufferlen) ;

		d2i_array (ubuf.dbuf, readcount, ptr + total, scale) ;
		total += readcount ;
		if (readcount < bufferlen)
			break ;
		len -= readcount ;
		} ;

	return total ;
} /* replace_read_d2i */