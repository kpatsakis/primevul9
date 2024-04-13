add_long_to_buf(long_u val, char_u *dst)
{
    int	    i;
    int	    shift;

    for (i = 1; i <= (int)sizeof(long_u); i++)
    {
	shift = 8 * (sizeof(long_u) - i);
	dst[i - 1] = (char_u) ((val >> shift) & 0xff);
    }
}