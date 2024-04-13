win_lbr_chartabsize(
    win_T	*wp,
    char_u	*line UNUSED, // start of the line
    char_u	*s,
    colnr_T	col,
    int		*headp UNUSED)
{
#ifdef FEAT_LINEBREAK
    int		c;
    int		size;
    colnr_T	col2;
    colnr_T	col_adj = 0; // col + screen size of tab
    colnr_T	colmax;
    int		added;
    int		mb_added = 0;
    int		numberextra;
    char_u	*ps;
    int		tab_corr = (*s == TAB);
    int		n;
    char_u	*sbr;

    /*
     * No 'linebreak', 'showbreak' and 'breakindent': return quickly.
     */
    if (!wp->w_p_lbr && !wp->w_p_bri && *get_showbreak_value(wp) == NUL)
#endif
    {
	if (wp->w_p_wrap)
	    return win_nolbr_chartabsize(wp, s, col, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, s, col)
    }

#ifdef FEAT_LINEBREAK
    /*
     * First get normal size, without 'linebreak'
     */
    size = win_chartabsize(wp, s, col);
    c = *s;
    if (tab_corr)
	col_adj = size - 1;

    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here
     */
    if (wp->w_p_lbr
	    && VIM_ISBREAK(c)
	    && !VIM_ISBREAK((int)s[1])
	    && wp->w_p_wrap
	    && wp->w_width != 0)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	numberextra = win_col_off(wp);
	col2 = col;
	colmax = (colnr_T)(wp->w_width - numberextra - col_adj);
	if (col >= colmax)
	{
	    colmax += col_adj;
	    n = colmax +  win_col_off2(wp);
	    if (n > 0)
		colmax += (((col - colmax) / n) + 1) * n - col_adj;
	}

	for (;;)
	{
	    ps = s;
	    MB_PTR_ADV(s);
	    c = *s;
	    if (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			    && (col2 == col || !VIM_ISBREAK((int)*ps))))))
		break;

	    col2 += win_chartabsize(wp, s, col2);
	    if (col2 >= colmax)		// doesn't fit
	    {
		size = colmax - col + col_adj;
		break;
	    }
	}
    }
    else if (has_mbyte && size == 2 && MB_BYTE2LEN(*s) > 1
				    && wp->w_p_wrap && in_win_border(wp, col))
    {
	++size;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at start of line.
     * Set *headp to the size of what we add.
     */
    added = 0;
    sbr = get_showbreak_value(wp);
    if ((*sbr != NUL || wp->w_p_bri) && wp->w_p_wrap && col != 0)
    {
	colnr_T sbrlen = 0;
	int	numberwidth = win_col_off(wp);

	numberextra = numberwidth;
	col += numberextra + mb_added;
	if (col >= (colnr_T)wp->w_width)
	{
	    col -= wp->w_width;
	    numberextra = wp->w_width - (numberextra - win_col_off2(wp));
	    if (col >= numberextra && numberextra > 0)
		col %= numberextra;
	    if (*sbr != NUL)
	    {
		sbrlen = (colnr_T)MB_CHARLEN(sbr);
		if (col >= sbrlen)
		    col -= sbrlen;
	    }
	    if (col >= numberextra && numberextra > 0)
		col = col % numberextra;
	    else if (col > 0 && numberextra > 0)
		col += numberwidth - win_col_off2(wp);

	    numberwidth -= win_col_off2(wp);
	}
	if (col == 0 || col + size + sbrlen > (colnr_T)wp->w_width)
	{
	    added = 0;
	    if (*sbr != NUL)
	    {
		if (size + sbrlen + numberwidth > (colnr_T)wp->w_width)
		{
		    // calculate effective window width
		    int width = (colnr_T)wp->w_width - sbrlen - numberwidth;
		    int prev_width = col
				 ? ((colnr_T)wp->w_width - (sbrlen + col)) : 0;

		    if (width <= 0)
			width = (colnr_T)1;
		    added += ((size - prev_width) / width) * vim_strsize(sbr);
		    if ((size - prev_width) % width)
			// wrapped, add another length of 'sbr'
			added += vim_strsize(sbr);
		}
		else
		    added += vim_strsize(sbr);
	    }
	    if (wp->w_p_bri)
		added += get_breakindent_win(wp, line);

	    size += added;
	    if (col != 0)
		added = 0;
	}
    }
    if (headp != NULL)
	*headp = added + mb_added;
    return size;
#endif
}