replace_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;

	/* FIXME : This is probably nowhere near optimal. */
	bufferlen = ARRAY_LEN (ubuf.dbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = psf_fread (ubuf.dbuf, sizeof (double), bufferlen, psf) ;

		if (psf->data_endswap == SF_TRUE)
			endswap_double_array (ubuf.dbuf, readcount) ;

		d2bd_read (ubuf.dbuf, readcount) ;

		memcpy (ptr + total, ubuf.dbuf, readcount * sizeof (double)) ;

		total += readcount ;
		if (readcount < bufferlen)
			break ;
		len -= readcount ;
		} ;

	return total ;
} /* replace_read_d */