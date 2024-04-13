update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    int		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_line_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	if (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    if (!doit || updating_screen
	    || State == ASKMORE || State == HITRETURN
	    || msg_scrolled
#ifdef FEAT_GUI
	    || gui.starting
#endif
	    || starting)
	return;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_redr_type != 0)
	    win_update(wp);
	if (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

    update_finish();
}