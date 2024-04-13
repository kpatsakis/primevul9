getdecchrs(void)
{
    long_u	nr = 0;
    int		c;
    int		i;

    for (i = 0; ; ++i)
    {
	c = regparse[0];
	if (c < '0' || c > '9')
	    break;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    if (i == 0)
	return -1;
    return (long)nr;
}