bd2d_write (double *buffer, int count)
{	while (--count >= 0)
	{	DOUBLE64_WRITE (buffer [count], (unsigned char*) (buffer + count)) ;
		} ;
} /* bd2d_write */