double64_peak_update	(SF_PRIVATE *psf, const double *buffer, int count, sf_count_t indx)
{	int 	chan ;
	int		k, position ;
	float	fmaxval ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	fmaxval = fabs (buffer [chan]) ;
		position = 0 ;
		for (k = chan ; k < count ; k += psf->sf.channels)
			if (fmaxval < fabs (buffer [k]))
			{	fmaxval = fabs (buffer [k]) ;
				position = k ;
				} ;

		if (fmaxval > psf->peak_info->peaks [chan].value)
		{	psf->peak_info->peaks [chan].value = fmaxval ;
			psf->peak_info->peaks [chan].position = psf->write_current + indx + (position / psf->sf.channels) ;
			} ;
		} ;

	return ;
} /* double64_peak_update */