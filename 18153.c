redraw_asap(int type)
{
    int		rows;
    int		cols = screen_Columns;
    int		r;
    int		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    int		i;
    u8char_T	*screenlineUC = NULL;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = NULL;	// copy from ScreenLines2[]

    redraw_later(type);
    if (msg_scrolled || (State != NORMAL && State != NORMAL_BUSY) || exiting)
	return ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    if (screenline == NULL || screenattr == NULL)
	ret = 2;
    if (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	if (screenlineUC == NULL)
	    ret = 2;
	for (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    if (screenlineC[i] == NULL)
		ret = 2;
	}
    }
    if (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	if (screenline2 == NULL)
	    ret = 2;
    }

    if (ret != 2)
    {
	// Save the text displayed in the command line area.
	for (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			ScreenLines + LineOffset[cmdline_row + r],
			(size_t)cols * sizeof(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + LineOffset[cmdline_row + r],
			(size_t)cols * sizeof(sattr_T));
	    if (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    ScreenLinesUC + LineOffset[cmdline_row + r],
			    (size_t)cols * sizeof(u8char_T));
		for (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				ScreenLinesC[i] + LineOffset[cmdline_row + r],
				(size_t)cols * sizeof(u8char_T));
	    }
	    if (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + LineOffset[cmdline_row + r],
			    (size_t)cols * sizeof(schar_T));
	}

	update_screen(0);
	ret = 3;

	if (must_redraw == 0)
	{
	    int	off = (int)(current_ScreenLine - ScreenLines);

	    // Restore the text displayed in the command line area.
	    for (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_ScreenLine,
			    screenline + r * cols,
			    (size_t)cols * sizeof(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * sizeof(sattr_T));
		if (enc_utf8)
		{
		    mch_memmove(ScreenLinesUC + off,
				screenlineUC + r * cols,
				(size_t)cols * sizeof(u8char_T));
		    for (i = 0; i < p_mco; ++i)
			mch_memmove(ScreenLinesC[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * sizeof(u8char_T));
		}
		if (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * sizeof(schar_T));
		screen_line(cmdline_row + r, 0, cols, cols, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    if (enc_utf8)
    {
	vim_free(screenlineUC);
	for (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    if (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    return ret;
}