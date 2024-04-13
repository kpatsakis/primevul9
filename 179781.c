find_match_char(int c, int ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    static pos_T pos_copy;
    int		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    if ((trypos = findmatchlimit(NULL, c, 0, ind_maxp_wk)) != NULL)
    {
	// check if the ( is in a // comment
	if ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (int)(cursor_save.lnum - trypos->lnum);
	    if (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		goto retry;
	    }
	    trypos = NULL;
	}
	else
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    if ((trypos_wk = ind_find_start_CORS(NULL)) != NULL) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (int)(cursor_save.lnum
			- trypos_wk->lnum);
		if (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    goto retry;
		}
		trypos = NULL;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    return trypos;
}