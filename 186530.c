bck_word(long count, int bigword, int stop)
{
    int		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    while (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	if (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	if (dec_cursor() == -1)		// started at start of file
	    return FAIL;

	if (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    while (cls() == 0)
	    {
		if (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		if (dec_cursor() == -1) // hit start of file, stop here
		    return OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    if (skip_chars(cls(), BACKWARD))
		return OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    return OK;
}