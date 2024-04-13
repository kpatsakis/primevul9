bckend_word(
    long	count,
    int		bigword,    // TRUE for "B"
    int		eol)	    // TRUE: stop at end of line.
{
    int		sclass;	    // starting class
    int		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    while (--count >= 0)
    {
	sclass = cls();
	if ((i = dec_cursor()) == -1)
	    return FAIL;
	if (eol && i == 1)
	    return OK;

	/*
	 * Move backward to before the start of this word.
	 */
	if (sclass != 0)
	{
	    while (cls() == sclass)
		if ((i = dec_cursor()) == -1 || (eol && i == 1))
		    return OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	while (cls() == 0)
	{
	    if (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		break;
	    if ((i = dec_cursor()) == -1 || (eol && i == 1))
		return OK;
	}
    }
    return OK;
}