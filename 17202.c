getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end)
{
    colnr_T	col;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    if (virtual_active())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &col, NULL, NULL);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
	if (pos->col < (colnr_T)STRLEN(ptr))
	{
	    int c = (*mb_ptr2char)(ptr + pos->col);

	    if (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		if (coladd > endadd)	// past end of line
		    endadd = 0;
		else
		    coladd = 0;
	    }
	}
	col += coladd;
	if (start != NULL)
	    *start = col;
	if (cursor != NULL)
	    *cursor = col;
	if (end != NULL)
	    *end = col + endadd;
    }
    else
	getvcol(wp, pos, start, cursor, end);
}