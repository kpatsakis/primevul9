match_suffix(char_u *fname)
{
    int		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (int)STRLEN(fname);
    setsuflen = 0;
    for (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	if (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    if (vim_strchr(tail, '.') == NULL)
	    {
		setsuflen = 1;
		break;
	    }
	}
	else
	{
	    if (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		break;
	    setsuflen = 0;
	}
    }
    return (setsuflen != 0);
}