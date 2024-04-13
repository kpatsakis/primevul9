getoctchrs(void)
{
    long_u	nr = 0;
    int		c;
    int		i;

    for (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	if (c < '0' || c > '7')
	    break;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    if (i == 0)
	return -1;
    return (long)nr;
}