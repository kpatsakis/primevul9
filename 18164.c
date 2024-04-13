update_prepare(void)
{
    cursor_off();
    updating_screen = TRUE;
#ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    if (gui.in_use)
	gui_undraw_cursor();
#endif
#ifdef FEAT_SEARCH_EXTRA
    start_search_hl();
#endif
#ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
#endif
}