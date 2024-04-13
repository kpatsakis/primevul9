ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    int		n;
    int		i;
    win_T	*wp;
    int		num_matches;
    char_u	**matches;
    char_u	*p;
    int		empty_fnum = 0;
    int		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    int		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    int		old_KeyTyped = KeyTyped;
#endif

    if (ERROR_IF_ANY_POPUP_WINDOW)
	return;

    if (eap != NULL)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	for (arg = eap->arg; *arg; ++arg)
	{
	    if (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		break;
	    }
	}
	arg = eap->arg;

	if (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_("E478: Don't panic!"));
	    return;
	}

	if (eap->skip)	    // not executing commands
	    return;
    }
    else
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    while (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    if (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &num_matches, &matches,
						 eap != NULL && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    if (n != FAIL && lang != NULL)
	// Find first item with the requested language.
	for (i = 0; i < num_matches; ++i)
	{
	    len = (int)STRLEN(matches[i]);
	    if (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		break;
	}
#endif
    if (i >= num_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	if (lang != NULL)
	    semsg(_("E661: Sorry, no '%s' help for %s"), lang, arg);
	else
#endif
	    semsg(_("E149: Sorry, no help for %s"), arg);
	if (n != FAIL)
	    FreeWild(num_matches, matches);
	return;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(num_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    if (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	if (cmdmod.cmod_tab != 0)
	    wp = NULL;
	else
	    FOR_ALL_WINDOWS(wp)
		if (bt_help(wp->w_buffer))
		    break;
	if (wp != NULL && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	else
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    if ((helpfd = mch_fopen((char *)p_hf, READBIN)) == NULL)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		goto erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    if (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= WSP_TOP;
	    if (win_split(0, n) == FAIL)
		goto erret;

	    if (curwin->w_height < p_hh)
		win_setheight((int)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (void)do_ecmd(0, NULL, NULL, NULL, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  NULL);  // buffer is still open, don't store info
	    if ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    if (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    KeyTyped = old_KeyTyped;
#endif

    if (tag != NULL)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    if (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	if (buf != NULL && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    if (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}