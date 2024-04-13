s2d_array (const short *src, double *dest, int count, double scale)
{	while (--count >= 0)
	{	dest [count] = scale * src [count] ;
		} ;
} /* s2d_array */