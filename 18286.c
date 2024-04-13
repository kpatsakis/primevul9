ins_copychar(linenr_T lnum)
{
    int		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *line;
    chartabsize_T   cts;

    if (lnum < 1 || lnum > curbuf->b_ml.ml_line_count)
    {
	vim_beep(BO_COPY);
	return NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    line = ml_get(lnum);
    prev_ptr = line;
    init_chartabsize_arg(&cts, curwin, lnum, 0, line, line);
    while (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    if (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    else
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    if (c == NUL)
	vim_beep(BO_COPY);
    return c;
}