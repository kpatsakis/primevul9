get_bytes_from_buf(char_u *buf, char_u *bytes, int num_bytes)
{
    int	    len = 0;
    int	    i;
    char_u  c;

    for (i = 0; i < num_bytes; i++)
    {
	if ((c = buf[len++]) == NUL)
	    return -1;
	if (c == K_SPECIAL)
	{
	    if (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		return -1;
	    if (buf[len++] == (int)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    if (buf[len++] == (int)KE_CSI)
		c = CSI;
	}
	else if (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (int)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	bytes[i] = c;
    }
    return len;
}