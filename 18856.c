set_termname(char_u *term)
{
    struct builtin_term *termp;
#ifdef HAVE_TGETENT
    int		builtin_first = p_tbi;
    int		try;
    int		termcap_cleared = FALSE;
#endif
    int		width = 0, height = 0;
    char	*error_msg = NULL;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    if (silent_mode)
	return OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    if (term_is_builtin(term))
    {
	term += 8;
#ifdef HAVE_TGETENT
	builtin_first = 1;
#endif
    }

/*
 * If HAVE_TGETENT is not defined, only the builtin termcap is used, otherwise:
 *   If builtin_first is TRUE:
 *     0. try builtin termcap
 *     1. try external termcap
 *     2. if both fail default to a builtin terminal
 *   If builtin_first is FALSE:
 *     1. try external termcap
 *     2. try builtin termcap, if both fail default to a builtin terminal
 */
#ifdef HAVE_TGETENT
    for (try = builtin_first ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	if (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    if ((error_msg = invoke_tgetent(tbuf, term)) == NULL)
	    {
		if (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_term_entries(&height, &width);
	    }
	}
	else	    // try == 0 || try == 2
#endif // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#ifdef HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    if (try == 2 && builtin_first && termcap_cleared)
		break;
#endif
	    /*
	     * search for 'term' in builtin_termcaps[]
	     */
	    termp = find_builtin_term(term);
	    if (termp->bt_string == NULL)	// did not find it
	    {
#ifdef HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		if (try == 0)			// try external one
		    continue;
		if (termcap_cleared)		// found in external termcap
		    break;
#endif
		report_term_error(error_msg, term);

		// when user typed :set term=xxx, quit here
		if (starting != NO_SCREEN)
		{
		    screen_start();	// don't know where cursor is now
		    wait_return(TRUE);
		    return FAIL;
		}
		term = DEFAULT_TERM;
		report_default_term(term);
		set_string_option_direct((char_u *)"term", -1, term,
								 OPT_FREE, 0);
		display_errors();
	    }
	    out_flush();
#ifdef HAVE_TGETENT
	    if (!termcap_cleared)
	    {
#endif
		clear_termoptions();	    // clear old options
#ifdef HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#endif
	    parse_builtin_tcap(term);
#ifdef FEAT_GUI
	    if (term_is_gui(term))
	    {
		out_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		if (!gui.in_use)
		    return FAIL;
#ifdef HAVE_TGETENT
		break;		// don't try using external termcap
#endif
	    }
#endif // FEAT_GUI
	}
#ifdef HAVE_TGETENT
    }
#endif

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    if (STRCMP(term, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    else
	T_CCS = empty_option;

#ifdef UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    if (!gui.in_use)
# endif
	get_stty();
#endif

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#ifdef FEAT_GUI
    if (!gui.in_use)
#endif
    {
	bs_p = find_termcode((char_u *)"kb");
	del_p = find_termcode((char_u *)"kD");
	if (bs_p == NULL || *bs_p == NUL)
	    add_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	if ((del_p == NULL || *del_p == NUL) &&
					    (bs_p == NULL || *bs_p != DEL))
	    add_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#if defined(UNIX) || defined(VMS)
    term_is_xterm = vim_is_xterm(term);
#endif
#ifdef FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_term_props(FALSE);
#endif

#if defined(UNIX) || defined(VMS)
    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	if (use_xterm_like_mouse(term))
	{
	    if (use_xterm_mouse())
		p = NULL;	// keep existing value, might be "xterm2"
	    else
		p = (char_u *)"xterm";
	}
# endif
	if (p != NULL)
	{
	    set_option_value_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_set((char_u *)"ttym");
	}
	if (p == NULL
#  ifdef FEAT_GUI
		|| gui.in_use
#  endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#else
    set_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#endif

#ifdef FEAT_MOUSE_XTERM
    // Focus reporting is supported by xterm compatible terminals and tmux.
    if (use_xterm_like_mouse(term))
    {
	char_u name[3];

	// handle focus in event
	name[0] = KS_EXTRA;
	name[1] = KE_FOCUSGAINED;
	name[2] = NUL;
	add_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	add_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#endif
#if (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#endif

#ifdef USE_TERM_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    if (STRCMP(term, DEFAULT_TERM) != 0)
	term_console = FALSE;
    else
    {
	term_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#endif

#if defined(UNIX) || defined(VMS)
    /*
     * 'ttyfast' is default on for xterm, iris-ansi and a few others.
     */
    if (vim_is_fastterm(term))
	p_tf = TRUE;
#endif
#ifdef USE_TERM_CONSOLE
    /*
     * 'ttyfast' is default on consoles
     */
    if (term_console)
	p_tf = TRUE;
#endif

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    full_screen = TRUE;		// we can use termcap codes from now on
    set_term_defaults();	// use current values as defaults
#ifdef FEAT_TERMRESPONSE
    LOG_TR(("setting crv_status to STATUS_GET"));
    crv_status.tr_progress = STATUS_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#endif

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    if (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    if (width <= 0 || height <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	width = 80;
#if defined(MSWIN)
	height = 25;	    // console is often 25 lines
#else
	height = 24;	    // most terminals are 24 lines
#endif
    }
    set_shellsize(width, height, FALSE);	// may change Rows
    if (starting != NO_SCREEN)
    {
	if (scroll_region)
	    scroll_region_reset();		// In case Rows changed
	check_map_keycodes();	// check mappings for terminal codes used

	{
	    buf_T	*buf;
	    aco_save_T	aco;

	    /*
	     * Execute the TermChanged autocommands for each buffer that is
	     * loaded.
	     */
	    FOR_ALL_BUFFERS(buf)
	    {
		if (curbuf->b_ml.ml_mfp != NULL)
		{
		    aucmd_prepbuf(&aco, buf);
		    apply_autocmds(EVENT_TERMCHANGED, NULL, NULL, FALSE,
								      curbuf);
		    // restore curwin/curbuf and a few other things
		    aucmd_restbuf(&aco);
		}
	    }
	}
    }

#ifdef FEAT_TERMRESPONSE
    may_req_termresponse();
#endif

    return OK;
}