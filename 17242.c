in_win_border(win_T *wp, colnr_T vcol)
{
    int		width1;		// width of first line (after line number)
    int		width2;		// width of further lines

    if (wp->w_width == 0)	// there is no border
	return FALSE;
    width1 = wp->w_width - win_col_off(wp);
    if ((int)vcol < width1 - 1)
	return FALSE;
    if ((int)vcol == width1 - 1)
	return TRUE;
    width2 = width1 + win_col_off2(wp);
    if (width2 <= 0)
	return FALSE;
    return ((vcol - width1) % width2 == width2 - 1);
}