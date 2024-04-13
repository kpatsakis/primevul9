replace_write_i2d	(SF_PRIVATE *psf, const int *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;
	double		scale ;

	scale = (psf->scale_int_float == 0) ? 1.0 : 1.0 / (8.0 * 0x10000000) ;
	bufferlen = ARRAY_LEN (ubuf.dbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		i2d_array (ptr + total, ubuf.dbuf, bufferlen, scale) ;

		if (psf->peak_info)
			double64_peak_update (psf, ubuf.dbuf, bufferlen, total / psf->sf.channels) ;

		bd2d_write (ubuf.dbuf, bufferlen) ;

		if (psf->data_endswap == SF_TRUE)
			endswap_double_array (ubuf.dbuf, bufferlen) ;

		writecount = psf_fwrite (ubuf.dbuf, sizeof (double), bufferlen, psf) ;
		total += writecount ;
		if (writecount < bufferlen)
			break ;
		len -= writecount ;
		} ;

	return total ;
} /* replace_write_i2d */