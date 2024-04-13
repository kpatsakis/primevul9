can_bs(
    int		what)	    /* BS_INDENT, BS_EOL or BS_START */
{
    switch (*p_bs)
    {
	case '2':	return TRUE;
	case '1':	return (what != BS_START);
	case '0':	return FALSE;
    }
    return vim_strchr(p_bs, what) != NULL;
}