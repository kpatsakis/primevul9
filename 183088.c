d2bd_read (double *buffer, int count)
{	while (--count >= 0)
	{	buffer [count] = DOUBLE64_READ ((unsigned char *) (buffer + count)) ;
		} ;
} /* d2bd_read */