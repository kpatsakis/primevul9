d2s_clip_array (const double *src, int count, short *dest, double scale)
{	while (--count >= 0)
	{	double tmp = scale * src [count] ;

		if (CPU_CLIPS_POSITIVE == 0 && tmp > 32767.0)
			dest [count] = SHRT_MAX ;
		else if (CPU_CLIPS_NEGATIVE == 0 && tmp < -32768.0)
			dest [count] = SHRT_MIN ;
		else
			dest [count] = lrint (tmp) ;
		} ;
} /* d2s_clip_array */