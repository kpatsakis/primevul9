textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	int	*rowp,	// screen row
	int	*scolp,	// start screen column
	int	*ccolp,	// cursor screen column
	int	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    int		row = 0;
    int		rowoff = 0;
    colnr_T	coloff = 0;

    if (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T off;
	colnr_T col;
	int     width;

	row = plines_m_win(wp, wp->w_topline, pos->lnum - 1) + 1;
	getvcol(wp, pos, &scol, &ccol, &ecol);

	// similar to what is done in validate_cursor_col()
	col = scol;
	off = win_col_off(wp);
	col += off;
	width = wp->w_width - off + win_col_off2(wp);

	// long line wrapping, adjust row
	if (wp->w_p_wrap
		&& col >= (colnr_T)wp->w_width
		&& width > 0)
	{
	    // use same formula as what is used in curs_columns()
	    rowoff = ((col - wp->w_width) / width + 1);
	    col -= rowoff * width;
	}
	col -= wp->w_leftcol;
	if (col >= wp->w_width)
	    col = -1;
	if (col >= 0 && row + rowoff <= wp->w_height)
	    coloff = col - scol + wp->w_wincol + 1;
	else
	    // character is left, right or below of the window
	    row = rowoff = scol = ccol = ecol = 0;
    }
    *rowp = W_WINROW(wp) + row + rowoff;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
}