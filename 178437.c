gethexchrs(int maxinputlen)
{
    long_u	nr = 0;
    int		c;
    int		i;

    for (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	if (!vim_isxdigit(c))
	    break;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    if (i == 0)
	return -1;
    return (long)nr;
}