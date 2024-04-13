ins_up(
    int		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    int		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    if (cursor_up(1L, TRUE) == OK)
    {
	if (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	if (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    else
	vim_beep(BO_CRSR);
}