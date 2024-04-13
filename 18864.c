find_term_bykeys(char_u *src)
{
    int		i;
    int		slen = (int)STRLEN(src);

    for (i = 0; i < tc_len; ++i)
    {
	if (slen == termcodes[i].len
			&& STRNCMP(termcodes[i].code, src, (size_t)slen) == 0)
	    return i;
    }
    return -1;
}