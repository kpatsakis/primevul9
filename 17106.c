set_topline(win_T *wp, linenr_T lnum)
{
#ifdef FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#endif

#ifdef FEAT_FOLDING
    // go to first of folded lines
    (void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
#endif
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    if (wp->w_botline > wp->w_buffer->b_ml.ml_line_count + 1)
	wp->w_botline = wp->w_buffer->b_ml.ml_line_count + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_set = TRUE;
#ifdef FEAT_DIFF
    if (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#endif
    wp->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_TOPLINE);
    // Don't set VALID_TOPLINE here, 'scrolloff' needs to be checked.
    redraw_later(VALID);
}