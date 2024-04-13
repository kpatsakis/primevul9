tltoa(unsigned long i)
{
    static char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    do
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    while (i > 0 && p > buf);
    return p;
}