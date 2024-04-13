host_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen ;
	sf_count_t	readcount, total = 0 ;

	readcount = psf_fread (ptr, sizeof (double), len, psf) ;

	if (psf->data_endswap != SF_TRUE)
		return readcount ;

	/* If the read length was sensible, endswap output in one go. */
	if (readcount < SENSIBLE_LEN)
	{	endswap_double_array (ptr, readcount) ;
		return readcount ;
		} ;

	bufferlen = SENSIBLE_LEN ;
	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;

		endswap_double_array (ptr + total, bufferlen) ;

		total += bufferlen ;
		len -= bufferlen ;
		} ;

	return total ;
} /* host_read_d */