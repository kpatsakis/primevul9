redraw_after_callback(int call_update_screen)
{
    ++redrawing_for_callback;

    if (State == HITRETURN || State == ASKMORE)
	; // do nothing
    else if (State & CMDLINE)
    {
	// Don't redraw when in prompt_for_number().
	if (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    if (msg_scrolled == 0
#ifdef FEAT_WILDMENU
		    && wild_menu_showing == 0
#endif
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    else if (State & (NORMAL | INSERT | TERMINAL))
    {
	// keep the command line if possible
	update_screen(VALID_NO_UPDATE);
	setcursor();

	if (msg_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    msg_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
#ifdef FEAT_GUI
    if (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    else
#endif
	out_flush();

    --redrawing_for_callback;
}