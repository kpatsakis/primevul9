d2s_array (const double *src, int count, short *dest, double scale)
{	while (--count >= 0)
	{	dest [count] = lrint (scale * src [count]) ;
		} ;
} /* d2s_array */