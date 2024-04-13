findpar(
    int		*pincl,	    // Return: TRUE if last char is to be included
    int		dir,
    long	count,
    int		what,
    int		both)
{
    linenr_T	curr;
    int		did_skip;   // TRUE after separating lines have been skipped
    int		first;	    // TRUE on first line
    int		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    int		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    while (count--)
    {
	did_skip = FALSE;
	for (first = TRUE; ; first = FALSE)
	{
	    if (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    if (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    if (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		break;

#ifdef FEAT_FOLDING
	    if (fold_skipped)
		curr -= dir;
#endif
	    if ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		if (count)
		    return FALSE;
		curr -= dir;
		break;
	    }
	}
    }
    setpcmark();
    if (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    if (curr == curbuf->b_ml.ml_line_count && what != '}')
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	if ((curwin->w_cursor.col = (colnr_T)STRLEN(line)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    else
	curwin->w_cursor.col = 0;
    return TRUE;
}