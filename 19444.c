vcol2col(win_T *wp, linenr_T lnum, int vcol)
{
    char_u	    *line;
    chartabsize_T   cts;

    // try to advance to the specified column
    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    while (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, NULL);
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    return (int)(cts.cts_ptr - line);
}