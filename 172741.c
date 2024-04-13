cmdline_handle_backslash_key(int c, int *gotesc)
{
    ++no_mapping;
    ++allow_keys;
    c = plain_vgetc();
    --no_mapping;
    --allow_keys;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    if (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && KeyTyped)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	return PROCESS_NEXT_KEY;
    }

#ifdef FEAT_EVAL
    if (c == 'e')
    {
	char_u	*p = NULL;
	int	len;

	/*
	 * Replace the command line with the result of an expression.
	 * Need to save and restore the current command line, to be
	 * able to enter a new one...
	 */
	if (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	else
	    new_cmdpos = ccline.cmdpos;

	c = get_expr_register();
	if (c == '=')
	{
	    // Need to save and restore ccline.  And set "textwinlock"
	    // to avoid nasty things like going to another buffer when
	    // evaluating an expression.
	    ++textwinlock;
	    p = get_expr_line();
	    --textwinlock;

	    if (p != NULL)
	    {
		len = (int)STRLEN(p);
		if (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    if (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    else
			ccline.cmdpos = new_cmdpos;

		    KeyTyped = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    return CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	return CMDLINE_NOT_CHANGED;
    }
#endif

    if (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    return GOTO_NORMAL_MODE;
}