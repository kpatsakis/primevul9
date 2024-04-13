finish_incsearch_highlighting(
	int gotesc,
	incsearch_state_T *is_state,
	int call_update_screen)
{
    if (is_state->did_incsearch)
    {
	is_state->did_incsearch = FALSE;
	if (gotesc)
	    curwin->w_cursor = is_state->save_cursor;
	else
	{
	    if (!EQUAL_POS(is_state->save_cursor, is_state->search_start))
	    {
		// put the '" mark at the original position
		curwin->w_cursor = is_state->save_cursor;
		setpcmark();
	    }
	    curwin->w_cursor = is_state->search_start;
	}
	restore_viewstate(&is_state->old_viewstate);
	highlight_match = FALSE;

	// by default search all lines
	search_first_line = 0;
	search_last_line = MAXLNUM;

	magic_overruled = is_state->magic_overruled_save;

	validate_cursor();	// needed for TAB
	redraw_all_later(UPD_SOME_VALID);
	if (call_update_screen)
	    update_screen(UPD_SOME_VALID);
    }
}