d2i_array (const double *src, int count, int *dest, double scale)
{	while (--count >= 0)
	{	dest [count] = lrint (scale * src [count]) ;
		} ;
} /* d2i_array */