d2i_clip_array (const double *src, int count, int *dest, double scale)
{	while (--count >= 0)
	{	float tmp = scale * src [count] ;

		if (CPU_CLIPS_POSITIVE == 0 && tmp > (1.0 * INT_MAX))
			dest [count] = INT_MAX ;
		else if (CPU_CLIPS_NEGATIVE == 0 && tmp < (-1.0 * INT_MAX))
			dest [count] = INT_MIN ;
		else
			dest [count] = lrint (tmp) ;
		} ;
} /* d2i_clip_array */