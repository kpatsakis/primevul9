i2d_array (const int *src, double *dest, int count, double scale)
{	while (--count >= 0)
	{	dest [count] = scale * src [count] ;
		} ;
} /* i2d_array */