check_top_offset(void)
{
    lineoff_T	loff;
    int		n;
    long        so = get_scrolloff_value();

    if (curwin->w_cursor.lnum < curwin->w_topline + so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#else
	n = 0;
#endif
	// Count the visible screen lines above the cursor line.
	while (n < so)
	{
	    topline_back(&loff);
	    // Stop when included a line above the window.
	    if (loff.lnum < curwin->w_topline
#ifdef FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#endif
		    )
		break;
	    n += loff.height;
	}
	if (n < so)
	    return TRUE;
    }
    return FALSE;
}