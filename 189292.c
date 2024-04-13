set_bool_option(
    int		opt_idx,		/* index in options[] table */
    char_u	*varp,			/* pointer to the option variable */
    int		value,			/* new value */
    int		opt_flags)		/* OPT_LOCAL and/or OPT_GLOBAL */
{
    int		old_value = *(int *)varp;

    /* Disallow changing some options from secure mode */
    if ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	return e_secure;

    *(int *)varp = value;	    /* set the new value */
#ifdef FEAT_EVAL
    /* Remember where the option was set. */
    set_option_scriptID_idx(opt_idx, opt_flags, current_SID);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /* May set global value for local option. */
    if ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(int *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    /*
     * Handle side effects of changing a bool option.
     */

    /* 'compatible' */
    if ((int *)varp == &p_cp)
    {
	compatible_set();
    }

#ifdef FEAT_LANGMAP
    if ((int *)varp == &p_lrm)
	/* 'langremap' -> !'langnoremap' */
	p_lnr = !p_lrm;
    else if ((int *)varp == &p_lnr)
	/* 'langnoremap' -> !'langremap' */
	p_lrm = !p_lnr;
#endif

#ifdef FEAT_PERSISTENT_UNDO
    /* 'undofile' */
    else if ((int *)varp == &curbuf->b_p_udf || (int *)varp == &p_udf)
    {
	/* Only take action when the option was set. When reset we do not
	 * delete the undo file, the option may be set again without making
	 * any changes in between. */
	if (curbuf->b_p_udf || p_udf)
	{
	    char_u	hash[UNDO_HASH_SIZE];
	    buf_T	*save_curbuf = curbuf;

	    FOR_ALL_BUFFERS(curbuf)
	    {
		/* When 'undofile' is set globally: for every buffer, otherwise
		 * only for the current buffer: Try to read in the undofile,
		 * if one exists, the buffer wasn't changed and the buffer was
		 * loaded */
		if ((curbuf == save_curbuf
				|| (opt_flags & OPT_GLOBAL) || opt_flags == 0)
			&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != NULL)
		{
		    u_compute_hash(hash);
		    u_read_undo(NULL, hash, curbuf->b_fname);
		}
	    }
	    curbuf = save_curbuf;
	}
    }
#endif

    else if ((int *)varp == &curbuf->b_p_ro)
    {
	/* when 'readonly' is reset globally, also reset readonlymode */
	if (!curbuf->b_p_ro && (opt_flags & OPT_LOCAL) == 0)
	    readonlymode = FALSE;

	/* when 'readonly' is set may give W10 again */
	if (curbuf->b_p_ro)
	    curbuf->b_did_warn = FALSE;

#ifdef FEAT_TITLE
	redraw_titles();
#endif
    }

#ifdef FEAT_GUI
    else if ((int *)varp == &p_mh)
    {
	if (!p_mh)
	    gui_mch_mousehide(FALSE);
    }
#endif

#ifdef FEAT_TITLE
    /* when 'modifiable' is changed, redraw the window title */
    else if ((int *)varp == &curbuf->b_p_ma)
    {
	redraw_titles();
    }
    /* when 'endofline' is changed, redraw the window title */
    else if ((int *)varp == &curbuf->b_p_eol)
    {
	redraw_titles();
    }
    /* when 'fixeol' is changed, redraw the window title */
    else if ((int *)varp == &curbuf->b_p_fixeol)
    {
	redraw_titles();
    }
# ifdef FEAT_MBYTE
    /* when 'bomb' is changed, redraw the window title and tab page text */
    else if ((int *)varp == &curbuf->b_p_bomb)
    {
	redraw_titles();
    }
# endif
#endif

    /* when 'bin' is set also set some other options */
    else if ((int *)varp == &curbuf->b_p_bin)
    {
	set_options_bin(old_value, curbuf->b_p_bin, opt_flags);
#ifdef FEAT_TITLE
	redraw_titles();
#endif
    }

#ifdef FEAT_AUTOCMD
    /* when 'buflisted' changes, trigger autocommands */
    else if ((int *)varp == &curbuf->b_p_bl && old_value != curbuf->b_p_bl)
    {
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						    NULL, NULL, TRUE, curbuf);
    }
#endif

    /* when 'swf' is set, create swapfile, when reset remove swapfile */
    else if ((int *)varp == &curbuf->b_p_swf)
    {
	if (curbuf->b_p_swf && p_uc)
	    ml_open_file(curbuf);		/* create the swap file */
	else
	    /* no need to reset curbuf->b_may_swap, ml_open_file() will check
	     * buf->b_p_swf */
	    mf_close_file(curbuf, TRUE);	/* remove the swap file */
    }

    /* when 'terse' is set change 'shortmess' */
    else if ((int *)varp == &p_terse)
    {
	char_u	*p;

	p = vim_strchr(p_shm, SHM_SEARCH);

	/* insert 's' in p_shm */
	if (p_terse && p == NULL)
	{
	    STRCPY(IObuff, p_shm);
	    STRCAT(IObuff, "s");
	    set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
	}
	/* remove 's' from p_shm */
	else if (!p_terse && p != NULL)
	    STRMOVE(p, p + 1);
    }

    /* when 'paste' is set or reset also change other options */
    else if ((int *)varp == &p_paste)
    {
	paste_option_changed();
    }

    /* when 'insertmode' is set from an autocommand need to do work here */
    else if ((int *)varp == &p_im)
    {
	if (p_im)
	{
	    if ((State & INSERT) == 0)
		need_start_insertmode = TRUE;
	    stop_insert_mode = FALSE;
	}
	/* only reset if it was set previously */
	else if (old_value)
	{
	    need_start_insertmode = FALSE;
	    stop_insert_mode = TRUE;
	    if (restart_edit != 0 && mode_displayed)
		clear_cmdline = TRUE;	/* remove "(insert)" */
	    restart_edit = 0;
	}
    }

    /* when 'ignorecase' is set or reset and 'hlsearch' is set, redraw */
    else if ((int *)varp == &p_ic && p_hls)
    {
	redraw_all_later(SOME_VALID);
    }

#ifdef FEAT_SEARCH_EXTRA
    /* when 'hlsearch' is set or reset: reset no_hlsearch */
    else if ((int *)varp == &p_hls)
    {
	SET_NO_HLSEARCH(FALSE);
    }
#endif

#ifdef FEAT_SCROLLBIND
    /* when 'scrollbind' is set: snapshot the current position to avoid a jump
     * at the end of normal_cmd() */
    else if ((int *)varp == &curwin->w_p_scb)
    {
	if (curwin->w_p_scb)
	{
	    do_check_scrollbind(FALSE);
	    curwin->w_scbind_pos = curwin->w_topline;
	}
    }
#endif

#if defined(FEAT_WINDOWS) && defined(FEAT_QUICKFIX)
    /* There can be only one window with 'previewwindow' set. */
    else if ((int *)varp == &curwin->w_p_pvw)
    {
	if (curwin->w_p_pvw)
	{
	    win_T	*win;

	    FOR_ALL_WINDOWS(win)
		if (win->w_p_pvw && win != curwin)
		{
		    curwin->w_p_pvw = FALSE;
		    return (char_u *)N_("E590: A preview window already exists");
		}
	}
    }
#endif

    /* when 'textmode' is set or reset also change 'fileformat' */
    else if ((int *)varp == &curbuf->b_p_tx)
    {
	set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, opt_flags);
    }

    /* when 'textauto' is set or reset also change 'fileformats' */
    else if ((int *)varp == &p_ta)
	set_string_option_direct((char_u *)"ffs", -1,
				 p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
						     OPT_FREE | opt_flags, 0);

    /*
     * When 'lisp' option changes include/exclude '-' in
     * keyword characters.
     */
#ifdef FEAT_LISP
    else if (varp == (char_u *)&(curbuf->b_p_lisp))
    {
	(void)buf_init_chartab(curbuf, FALSE);	    /* ignore errors */
    }
#endif

#ifdef FEAT_TITLE
    /* when 'title' changed, may need to change the title; same for 'icon' */
    else if ((int *)varp == &p_title)
    {
	did_set_title(FALSE);
    }

    else if ((int *)varp == &p_icon)
    {
	did_set_title(TRUE);
    }
#endif

    else if ((int *)varp == &curbuf->b_changed)
    {
	if (!value)
	    save_file_ff(curbuf);	/* Buffer is unchanged */
#ifdef FEAT_TITLE
	redraw_titles();
#endif
#ifdef FEAT_AUTOCMD
	modified_was_set = value;
#endif
    }

#ifdef BACKSLASH_IN_FILENAME
    else if ((int *)varp == &p_ssl)
    {
	if (p_ssl)
	{
	    psepc = '/';
	    psepcN = '\\';
	    pseps[0] = '/';
	}
	else
	{
	    psepc = '\\';
	    psepcN = '/';
	    pseps[0] = '\\';
	}

	/* need to adjust the file name arguments and buffer names. */
	buflist_slash_adjust();
	alist_slash_adjust();
# ifdef FEAT_EVAL
	scriptnames_slash_adjust();
# endif
    }
#endif

    /* If 'wrap' is set, set w_leftcol to zero. */
    else if ((int *)varp == &curwin->w_p_wrap)
    {
	if (curwin->w_p_wrap)
	    curwin->w_leftcol = 0;
    }

#ifdef FEAT_WINDOWS
    else if ((int *)varp == &p_ea)
    {
	if (p_ea && !old_value)
	    win_equal(curwin, FALSE, 0);
    }
#endif

    else if ((int *)varp == &p_wiv)
    {
	/*
	 * When 'weirdinvert' changed, set/reset 't_xs'.
	 * Then set 'weirdinvert' according to value of 't_xs'.
	 */
	if (p_wiv && !old_value)
	    T_XS = (char_u *)"y";
	else if (!p_wiv && old_value)
	    T_XS = empty_option;
	p_wiv = (*T_XS != NUL);
    }

#ifdef FEAT_BEVAL
    else if ((int *)varp == &p_beval)
    {
	if (p_beval && !old_value)
	    gui_mch_enable_beval_area(balloonEval);
	else if (!p_beval && old_value)
	    gui_mch_disable_beval_area(balloonEval);
    }
#endif

#ifdef FEAT_AUTOCHDIR
    else if ((int *)varp == &p_acd)
    {
	/* Change directories when the 'acd' option is set now. */
	DO_AUTOCHDIR
    }
#endif

#ifdef FEAT_DIFF
    /* 'diff' */
    else if ((int *)varp == &curwin->w_p_diff)
    {
	/* May add or remove the buffer from the list of diff buffers. */
	diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
	if (foldmethodIsDiff(curwin))
	    foldUpdateAll(curwin);
# endif
    }
#endif

#ifdef USE_IM_CONTROL
    /* 'imdisable' */
    else if ((int *)varp == &p_imdisable)
    {
	/* Only de-activate it here, it will be enabled when changing mode. */
	if (p_imdisable)
	    im_set_active(FALSE);
	else if (State & INSERT)
	    /* When the option is set from an autocommand, it may need to take
	     * effect right away. */
	    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    }
#endif

#ifdef FEAT_SPELL
    /* 'spell' */
    else if ((int *)varp == &curwin->w_p_spell)
    {
	if (curwin->w_p_spell)
	{
	    char_u	*errmsg = did_set_spelllang(curwin);
	    if (errmsg != NULL)
		EMSG(_(errmsg));
	}
    }
#endif

#ifdef FEAT_FKMAP
    else if ((int *)varp == &p_altkeymap)
    {
	if (old_value != p_altkeymap)
	{
	    if (!p_altkeymap)
	    {
		p_hkmap = p_fkmap;
		p_fkmap = 0;
	    }
	    else
	    {
		p_fkmap = p_hkmap;
		p_hkmap = 0;
	    }
	    (void)init_chartab();
	}
    }

    /*
     * In case some second language keymapping options have changed, check
     * and correct the setting in a consistent way.
     */

    /*
     * If hkmap or fkmap are set, reset Arabic keymapping.
     */
    if ((p_hkmap || p_fkmap) && p_altkeymap)
    {
	p_altkeymap = p_fkmap;
# ifdef FEAT_ARABIC
	curwin->w_p_arab = FALSE;
# endif
	(void)init_chartab();
    }

    /*
     * If hkmap set, reset Farsi keymapping.
     */
    if (p_hkmap && p_altkeymap)
    {
	p_altkeymap = 0;
	p_fkmap = 0;
# ifdef FEAT_ARABIC
	curwin->w_p_arab = FALSE;
# endif
	(void)init_chartab();
    }

    /*
     * If fkmap set, reset Hebrew keymapping.
     */
    if (p_fkmap && !p_altkeymap)
    {
	p_altkeymap = 1;
	p_hkmap = 0;
# ifdef FEAT_ARABIC
	curwin->w_p_arab = FALSE;
# endif
	(void)init_chartab();
    }
#endif

#ifdef FEAT_ARABIC
    if ((int *)varp == &curwin->w_p_arab)
    {
	if (curwin->w_p_arab)
	{
	    /*
	     * 'arabic' is set, handle various sub-settings.
	     */
	    if (!p_tbidi)
	    {
		/* set rightleft mode */
		if (!curwin->w_p_rl)
		{
		    curwin->w_p_rl = TRUE;
		    changed_window_setting();
		}

		/* Enable Arabic shaping (major part of what Arabic requires) */
		if (!p_arshape)
		{
		    p_arshape = TRUE;
		    redraw_later_clear();
		}
	    }

	    /* Arabic requires a utf-8 encoding, inform the user if its not
	     * set. */
	    if (STRCMP(p_enc, "utf-8") != 0)
	    {
		static char *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

		msg_source(hl_attr(HLF_W));
		MSG_ATTR(_(w_arabic), hl_attr(HLF_W));
#ifdef FEAT_EVAL
		set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
#endif
	    }

# ifdef FEAT_MBYTE
	    /* set 'delcombine' */
	    p_deco = TRUE;
# endif

# ifdef FEAT_KEYMAP
	    /* Force-set the necessary keymap for arabic */
	    set_option_value((char_u *)"keymap", 0L, (char_u *)"arabic",
								   OPT_LOCAL);
# endif
# ifdef FEAT_FKMAP
	    p_altkeymap = 0;
	    p_hkmap = 0;
	    p_fkmap = 0;
	    (void)init_chartab();
# endif
	}
	else
	{
	    /*
	     * 'arabic' is reset, handle various sub-settings.
	     */
	    if (!p_tbidi)
	    {
		/* reset rightleft mode */
		if (curwin->w_p_rl)
		{
		    curwin->w_p_rl = FALSE;
		    changed_window_setting();
		}

		/* 'arabicshape' isn't reset, it is a global option and
		 * another window may still need it "on". */
	    }

	    /* 'delcombine' isn't reset, it is a global option and another
	     * window may still want it "on". */

# ifdef FEAT_KEYMAP
	    /* Revert to the default keymap */
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    curbuf->b_p_imsearch = B_IMODE_USE_INSERT;
# endif
	}
    }

#endif

#ifdef FEAT_TERMGUICOLORS
    /* 'termguicolors' */
    else if ((int *)varp == &p_tgc)
    {
# ifdef FEAT_GUI
	if (!gui.in_use && !gui.starting)
# endif
	    highlight_gui_started();
    }
#endif

    /*
     * End of handling side effects for bool options.
     */

    /* after handling side effects, call autocommand */

    options[opt_idx].flags |= P_WAS_SET;

#if defined(FEAT_AUTOCMD) && defined(FEAT_EVAL)
    if (!starting)
    {
	char_u buf_old[2], buf_new[2], buf_type[7];
	vim_snprintf((char *)buf_old, 2, "%d", old_value ? TRUE: FALSE);
	vim_snprintf((char *)buf_new, 2, "%d", value ? TRUE: FALSE);
	vim_snprintf((char *)buf_type, 7, "%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
	set_vim_var_string(VV_OPTION_NEW, buf_new, -1);
	set_vim_var_string(VV_OPTION_OLD, buf_old, -1);
	set_vim_var_string(VV_OPTION_TYPE, buf_type, -1);
	apply_autocmds(EVENT_OPTIONSET, (char_u *) options[opt_idx].fullname, NULL, FALSE, NULL);
	reset_v_option_vars();
    }
#endif

    comp_col();			    /* in case 'ruler' or 'showcmd' changed */
    if (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0)
	curwin->w_set_curswant = TRUE;
    check_redraw(options[opt_idx].flags);

    return NULL;
}