replace_write_d	(SF_PRIVATE *psf, const double *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;

	/* FIXME : This is probably nowhere near optimal. */
	if (psf->peak_info)
		double64_peak_update (psf, ptr, len, 0) ;

	bufferlen = ARRAY_LEN (ubuf.dbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;

		memcpy (ubuf.dbuf, ptr + total, bufferlen * sizeof (double)) ;

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
} /* replace_write_d */