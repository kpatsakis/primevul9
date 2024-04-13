lbr_chartabsize(
    char_u		*line UNUSED, // start of the line
    unsigned char	*s,
    colnr_T		col)
{
#ifdef FEAT_LINEBREAK
    if (!curwin->w_p_lbr && *get_showbreak_value(curwin) == NUL
							   && !curwin->w_p_bri)
    {
#endif
	if (curwin->w_p_wrap)
	    return win_nolbr_chartabsize(curwin, s, col, NULL);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, s, col)
#ifdef FEAT_LINEBREAK
    }
    return win_lbr_chartabsize(curwin, line == NULL ? s : line, s, col, NULL);
#endif
}