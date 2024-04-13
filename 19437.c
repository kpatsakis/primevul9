get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    int		row = mouse_row;
    int		col = mouse_col;

    if (row < 0 || col < 0)		// check if it makes sense
	return IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    if (wp == NULL)
	return IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    if (row >= wp->w_height)	// In (or below) status line
	return IN_STATUS_LINE;
    if (col >= wp->w_width)	// In vertical separator line
	return IN_SEP_LINE;

    if (wp != curwin)
	return IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    if (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, NULL))
	return IN_STATUS_LINE; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col);

    if (mpos->col > 0)
	--mpos->col;
    mpos->coladd = 0;
    return IN_BUFFER;
}