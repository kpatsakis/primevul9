check_colorcolumn(win_T *wp)
{
    char_u	*s;
    int		col;
    int		count = 0;
    int		color_cols[256];
    int		i;
    int		j = 0;

    if (wp->w_buffer == NULL)
	return NULL;  /* buffer was closed */

    for (s = wp->w_p_cc; *s != NUL && count < 255;)
    {
	if (*s == '-' || *s == '+')
	{
	    /* -N and +N: add to 'textwidth' */
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    if (!VIM_ISDIGIT(*s))
		return e_invarg;
	    col = col * getdigits(&s);
	    if (wp->w_buffer->b_p_tw == 0)
		goto skip;  /* 'textwidth' not set, skip this item */
	    col += wp->w_buffer->b_p_tw;
	    if (col < 0)
		goto skip;
	}
	else if (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	else
	    return e_invarg;
	color_cols[count++] = col - 1;  /* 1-based to 0-based */
skip:
	if (*s == NUL)
	    break;
	if (*s != ',')
	    return e_invarg;
	if (*++s == NUL)
	    return e_invarg;  /* illegal trailing comma as in "set cc=80," */
    }

    vim_free(wp->w_p_cc_cols);
    if (count == 0)
	wp->w_p_cc_cols = NULL;
    else
    {
	wp->w_p_cc_cols = (int *)alloc((unsigned)sizeof(int) * (count + 1));
	if (wp->w_p_cc_cols != NULL)
	{
	    /* sort the columns for faster usage on screen redraw inside
	     * win_line() */
	    qsort(color_cols, count, sizeof(int), int_cmp);

	    for (i = 0; i < count; ++i)
		/* skip duplicates */
		if (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  /* end marker */
	}
    }

    return NULL;  /* no error */
}