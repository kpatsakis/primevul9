gettail(char_u *fname)
{
    char_u  *p1, *p2;

    if (fname == NULL)
	return (char_u *)"";
    for (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	if (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    return p1;
}