host_read_d2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	void		(*convert) (const double *, int, short *, double) ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;
	double		scale ;

	convert = (psf->add_clipping) ? d2s_clip_array : d2s_array ;
	bufferlen = ARRAY_LEN (ubuf.dbuf) ;
	scale = (psf->float_int_mult == 0) ? 1.0 : 0x7FFF / psf->float_max ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = psf_fread (ubuf.dbuf, sizeof (double), bufferlen, psf) ;

		if (psf->data_endswap == SF_TRUE)
			endswap_double_array (ubuf.dbuf, readcount) ;

		convert (ubuf.dbuf, readcount, ptr + total, scale) ;
		total += readcount ;
		len -= readcount ;
		if (readcount < bufferlen)
			break ;
		} ;

	return total ;
} /* host_read_d2s */