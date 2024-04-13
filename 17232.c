win_nolbr_chartabsize(
    win_T	*wp,
    char_u	*s,
    colnr_T	col,
    int		*headp)
{
    int		n;

    if (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
# ifdef FEAT_VARTABS
	return tabstop_padding(col, wp->w_buffer->b_p_ts,
				    wp->w_buffer->b_p_vts_array);
# else
	n = wp->w_buffer->b_p_ts;
	return (int)(n - (col % n));
# endif
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    if (n == 2 && MB_BYTE2LEN(*s) > 1 && in_win_border(wp, col))
    {
	if (headp != NULL)
	    *headp = 1;
	return 3;
    }
    return n;
}