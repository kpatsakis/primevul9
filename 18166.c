win_redr_ruler(win_T *wp, int always, int ignore_pum)
{
#define RULER_BUF_LEN 70
    char_u	buffer[RULER_BUF_LEN];
    int		row;
    int		fillchar;
    int		attr;
    int		empty_line = FALSE;
    colnr_T	virtcol;
    int		i;
    size_t	len;
    int		o;
    int		this_ru_col;
    int		off = 0;
    int		width;

    // If 'ruler' off or redrawing disabled, don't do anything
    if (!p_ru)
	return;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    if (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_line_count)
	return;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    if (wp == lastwin && lastwin->w_status_height == 0)
	if (edit_submode != NULL)
	    return;
    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    if (!ignore_pum && pum_visible())
	return;

#ifdef FEAT_STL_OPT
    if (*p_ruf)
    {
	int	called_emsg_before = called_emsg;

	win_redr_custom(wp, TRUE);
	if (called_emsg > called_emsg_before)
	    set_string_option_direct((char_u *)"rulerformat", -1,
					   (char_u *)"", OPT_FREE, SID_ERROR);
	return;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    if (!(State & INSERT)
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_line = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    if (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_line_count != wp->w_ru_line_count
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_line != wp->w_ru_empty)
    {
	cursor_off();
	if (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	else
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    width = Columns;
	    off = 0;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	if (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, NULL, &virtcol, NULL);
	    wp->w_p_list = TRUE;
	}

	/*
	 * Some sprintfs return the length, some return a pointer.
	 * To avoid portability problems we use strlen() here.
	 */
	vim_snprintf((char *)buffer, RULER_BUF_LEN, "%ld,",
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	len = STRLEN(buffer);
	col_print(buffer + len, RULER_BUF_LEN - len,
			empty_line ? 0 : (int)wp->w_cursor.col + 1,
			(int)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	i = (int)STRLEN(buffer);
	get_rel_pos(wp, buffer + i + 1, RULER_BUF_LEN - i - 1);
	o = i + vim_strsize(buffer + i + 1);
	if (wp->w_status_height == 0)	// can't use last char of screen
	    ++o;
	this_ru_col = ru_col - (Columns - width);
	if (this_ru_col < 0)
	    this_ru_col = 0;
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	if (this_ru_col < (width + 1) / 2)
	    this_ru_col = (width + 1) / 2;
	if (this_ru_col + o < width)
	{
	    // need at least 3 chars left for get_rel_pos() + NUL
	    while (this_ru_col + o < width && RULER_BUF_LEN > i + 4)
	    {
		if (has_mbyte)
		    i += (*mb_char2bytes)(fillchar, buffer + i);
		else
		    buffer[i++] = fillchar;
		++o;
	    }
	    get_rel_pos(wp, buffer + i, RULER_BUF_LEN - i);
	}
	// Truncate at window boundary.
	if (has_mbyte)
	{
	    o = 0;
	    for (i = 0; buffer[i] != NUL; i += (*mb_ptr2len)(buffer + i))
	    {
		o += (*mb_ptr2cells)(buffer + i);
		if (this_ru_col + o > width)
		{
		    buffer[i] = NUL;
		    break;
		}
	    }
	}
	else if (this_ru_col + (int)STRLEN(buffer) > width)
	    buffer[width - this_ru_col] = NUL;

	screen_puts(buffer, row, this_ru_col + off, attr);
	i = redraw_cmdline;
	screen_fill(row, row + 1,
		this_ru_col + off + (int)STRLEN(buffer),
		(int)(off + width),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = i;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_empty = empty_line;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_line_count = wp->w_buffer->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif
    }
}