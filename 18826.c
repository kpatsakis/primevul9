get_long_from_buf(char_u *buf, long_u *val)
{
    int	    len;
    char_u  bytes[sizeof(long_u)];
    int	    i;
    int	    shift;

    *val = 0;
    len = get_bytes_from_buf(buf, bytes, (int)sizeof(long_u));
    if (len != -1)
    {
	for (i = 0; i < (int)sizeof(long_u); i++)
	{
	    shift = 8 * (sizeof(long_u) - 1 - i);
	    *val += (long_u)bytes[i] << shift;
	}
    }
    return len;
}