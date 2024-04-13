cmdline_toggle_langmap(long *b_im_ptr)
{
    if (map_to_exists_mode((char_u *)"", LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= LANGMAP;
#ifdef HAVE_INPUT_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	if (b_im_ptr != NULL)
	{
	    if (State & LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    else
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_INPUT_METHOD
    else
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	if ((p_imdisable && b_im_ptr != NULL)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    if (b_im_ptr != NULL)
		*b_im_ptr = B_IMODE_NONE;
	}
	else
	{
	    im_set_active(TRUE);	// Enable input method
	    if (b_im_ptr != NULL)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    if (b_im_ptr != NULL)
    {
	if (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	else
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#if defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}