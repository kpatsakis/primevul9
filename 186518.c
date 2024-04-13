end_word(
    long	count,
    int		bigword,
    int		stop,
    int		empty)
{
    int		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    while (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	if (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	if (inc_cursor() == -1)
	    return FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	if (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    if (skip_chars(sclass, FORWARD))
		return FAIL;
	}
	else if (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    while (cls() == 0)
	    {
		if (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		if (inc_cursor() == -1)	    // hit end of file, stop here
		    return FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    if (skip_chars(cls(), FORWARD))
		return FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    return OK;
}