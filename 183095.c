replace_write_f2d	(SF_PRIVATE *psf, const float *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;

	bufferlen = ARRAY_LEN (ubuf.dbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		f2d_array (ptr + total, ubuf.dbuf, bufferlen) ;

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
} /* replace_write_f2d */