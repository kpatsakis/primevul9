ins_mouse(int c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

# ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    if (!gui.in_use)
# endif
	if (!mouse_has(MOUSE_INSERT))
	    return;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    if (do_mouse(NULL, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	if (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    if (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_insert = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : NULL);
	if (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_buffer;
	}
	set_can_cindent(TRUE);
    }

    // redraw status lines (in case another window became active)
    redraw_statuslines();
}