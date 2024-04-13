host_write_d	(SF_PRIVATE *psf, const double *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;

	if (psf->peak_info)
		double64_peak_update (psf, ptr, len, 0) ;

	if (psf->data_endswap != SF_TRUE)
		return psf_fwrite (ptr, sizeof (double), len, psf) ;

	bufferlen = ARRAY_LEN (ubuf.dbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;

		endswap_double_copy (ubuf.dbuf, ptr + total, bufferlen) ;

		writecount = psf_fwrite (ubuf.dbuf, sizeof (double), bufferlen, psf) ;
		total += writecount ;
		if (writecount < bufferlen)
			break ;
		len -= writecount ;
		} ;

	return total ;
} /* host_write_d */