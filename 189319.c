briopt_check(win_T *wp)
{
    char_u	*p;
    int		bri_shift = 0;
    long	bri_min = 20;
    int		bri_sbr = FALSE;

    p = wp->w_p_briopt;
    while (*p != NUL)
    {
	if (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	else if (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	else if (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	if (*p != ',' && *p != NUL)
	    return FAIL;
	if (*p == ',')
	    ++p;
    }

    wp->w_p_brishift = bri_shift;
    wp->w_p_brimin   = bri_min;
    wp->w_p_brisbr   = bri_sbr;

    return OK;
}