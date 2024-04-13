update_finish(void)
{
    if (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_SEARCH_EXTRA
    end_search_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    if (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}