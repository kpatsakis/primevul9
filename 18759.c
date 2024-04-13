edit(
    int		cmdchar,
    int		startln,	// if set, insert at start of line
    long	count)
{
    int		c = 0;
    char_u	*ptr;
    int		lastc = 0;
    int		mincol;
    static linenr_T o_lnum = 0;
    int		i;
    int		did_backspace = TRUE;	    // previous char was backspace
#ifdef FEAT_CINDENT
    int		line_is_white = FALSE;	    // line is empty before insert
#endif
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    int		old_topfill = -1;
#endif
    int		inserted_space = FALSE;     // just inserted a space
    int		replaceState = REPLACE;
    int		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    int		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    int		cursor_line_was_concealed;
#endif

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    if (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	return FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    if (textwinlock != 0 || textlock != 0
			  || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	return FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    if (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	if (cmdchar == 'R')
	    ptr = (char_u *)"r";
	else if (cmdchar == 'V')
	    ptr = (char_u *)"v";
	else
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, NULL, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	if (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	if (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_line_count)
	{
	    int save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = INSERT;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_line_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_line(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    if (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    else
    {
	Insstart = curwin->w_cursor;
	if (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    if (!did_ai)
	ai_col = 0;

    if (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	AppendNumberToRedobuff(count);
	if (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	else
	{
	    if (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    else
		AppendCharToRedobuff(cmdchar);
	    if (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    else if (cmdchar == 'r')	    // "r<CR>" command
		count = 1;		    // insert only one <CR>
	}
    }

    if (cmdchar == 'R')
    {
	State = REPLACE;
    }
    else if (cmdchar == 'V' || cmdchar == 'v')
    {
	State = VREPLACE;
	replaceState = VREPLACE;
	orig_line_count = curbuf->b_ml.ml_line_count;
	vr_lines_changed = 1;
    }
    else
	State = INSERT;

    trigger_modechanged();
    stop_insert_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_line(cursor_line_was_concealed);
#endif

    /*
     * Need to recompute the cursor position, it might move when the cursor is
     * on a TAB or special character.
     */
    curs_columns(TRUE);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    if (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= LANGMAP;
#ifdef HAVE_INPUT_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
#ifdef FEAT_CMDL_INFO
    clear_showcmd();
#endif
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == INSERT && p_ri);
    if (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    if (!p_ek)
    {
#ifdef FEAT_JOB_CHANNEL
	ch_log_output = TRUE;
#endif
	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	out_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	out_str(T_CTE);
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    if (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	if (where_paste_started.lnum)
	    arrow_used = FALSE;
	else
	    arrow_used = TRUE;
	restart_edit = 0;

	/*
	 * If the cursor was after the end-of-line before the CTRL-O and it is
	 * now at the end-of-line, put it after the end-of-line (this is not
	 * correct in very rare cases).
	 * Also do this if curswant is greater than the current virtual
	 * column.  Eg after "^O$" or "^O80|".
	 */
	validate_virtcol();
	update_curswant();
	if (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    if (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    else if (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		if (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    else
	arrow_used = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_start_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
#ifdef FEAT_CINDENT
    can_cindent = TRUE;
#endif
#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    if (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    if (p_smd && msg_silent == 0)
	i = showmode();

    if (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
#ifdef FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#endif

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    ptr = get_inserted();
    if (ptr == NULL)
	new_insert_skip = 0;
    else
    {
	new_insert_skip = (int)STRLEN(ptr);
	vim_free(ptr);
    }

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    for (;;)
    {
#ifdef FEAT_RIGHTLEFT
	if (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	else
	    revins_legal = 0;
#endif
	if (arrow_used)	    // don't repeat insert when arrow key used
	    count = 0;

	if (update_Insstart_orig)
	    Insstart_orig = Insstart;

	if (stop_insert_mode && !pum_visible())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    count = 0;
	    goto doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	if (!arrow_used)
	    curwin->w_set_curswant = TRUE;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	if (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    if (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	msg_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	if (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	if (fdo_flags & FDO_INSERT)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	if (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	if (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#endif

	/*
	 * If we inserted a character at the last position of the last line in
	 * the window, scroll the window one line up. This avoids an extra
	 * redraw.
	 * This is detected when the cursor column is smaller after inserting
	 * something.
	 * Don't do this when the topline changed already, it has
	 * already been adjusted (by insertchar() calling open_line())).
	 */
	if (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    if (
#ifdef FEAT_VARTABS
		(int)curwin->w_wcol < mincol - tabstop_at(
					  get_nolist_virtcol(), curbuf->b_p_ts,
							 curbuf->b_p_vts_array)
#else
		(int)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow == W_WINROW(curwin)
				 + curwin->w_height - 1 - get_scrolloff_value()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		if (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		else
#endif
#ifdef FEAT_FOLDING
		if (hasFolding(curwin->w_topline, NULL, &old_topline))
		    set_topline(curwin, old_topline + 1);
		else
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	update_topline();

	did_backspace = FALSE;

	validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	if (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	if (curwin->w_p_crb)
	    do_check_cursorbind();
	update_curswant();
	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	old_topfill = curwin->w_topfill;
#endif

#ifdef USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#endif

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	if (c != K_CURSORHOLD)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	if (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	else
	    dont_sync_undo = FALSE;
	if (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	else
	    do
	    {
		c = safe_vgetc();

		if (stop_insert_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    if (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    count = 0;
		    nomove = TRUE;
		    ins_compl_prep(ESC);
		    goto doESCkey;
		}
	    } while (c == K_IGNORE || c == K_NOP);

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;

#ifdef FEAT_RIGHTLEFT
	if (p_hkmap && KeyTyped)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	if (KeyTyped && !KeyStuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	if (ins_compl_active()
		&& pum_wanted()
		&& curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match())
	{
	    // BS: Delete one character from "compl_leader".
	    if ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		continue;

	    // When no match was selected or it was edited.
	    if (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		if (c == Ctrl_L
			&& (!ctrl_x_mode_line_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    continue;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		if (ins_compl_accept_char(c))
		{
#if defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_insert_char_pre(c);
		    char_u *p;

		    if (str != NULL)
		    {
			for (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    else
#endif
			ins_compl_addleader(c);
		    continue;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		if ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    ins_compl_insert(FALSE);
		}
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	if (ins_compl_prep(c))
	    continue;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	if (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_keys;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_keys;
	    if (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    else if (c == Ctrl_G && p_im)
		continue;
	    else
	    {
		if (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		count = 0;
		goto doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	if ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    goto docomplete;
	if (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    continue;
	}

#ifdef FEAT_CINDENT
	if (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    line_is_white = inindent(0);
	    if (in_cinkeys(c, '!', line_is_white))
		goto force_cindent;
	    if (can_cindent && in_cinkeys(c, '*', line_is_white)
							&& stop_arrow() == OK)
		do_c_expr_indent();
	}
#endif

#ifdef FEAT_RIGHTLEFT
	if (curwin->w_p_rl)
	    switch (c)
	    {
		case K_LEFT:	c = K_RIGHT; break;
		case K_S_LEFT:	c = K_S_RIGHT; break;
		case K_C_LEFT:	c = K_C_RIGHT; break;
		case K_RIGHT:	c = K_LEFT; break;
		case K_S_RIGHT: c = K_S_LEFT; break;
		case K_C_RIGHT: c = K_C_LEFT; break;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	if (ins_start_select(c))
	    continue;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	switch (c)
	{
	case ESC:	// End input mode
	    if (echeck_abbr(ESC + ABBR_OFF))
		break;
	    // FALLTHROUGH

	case Ctrl_C:	// End input mode
#ifdef FEAT_CMDWIN
	    if (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		goto doESCkey;
	    }
#endif
#ifdef FEAT_JOB_CHANNEL
	    if (c == Ctrl_C && bt_prompt(curbuf))
	    {
		if (invoke_prompt_interrupt())
		{
		    if (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			goto doESCkey;
		    break;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    if (goto_im())
	    {
		if (got_int)
		{
		    (void)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		else
		    vim_beep(BO_IM);
		break;
	    }
doESCkey:
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    if (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    if (ins_esc(&count, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		if (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;
		return (c == Ctrl_O);
	    }
	    continue;

	case Ctrl_Z:	// suspend when 'insertmode' set
	    if (!p_im)
		goto normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    continue;

	case Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    if (ctrl_x_mode_omni())
		goto docomplete;
#endif
	    if (echeck_abbr(Ctrl_O + ABBR_OFF))
		break;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    if (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    count = 0;
	    goto doESCkey;

	case K_INS:	// toggle insert/replace mode
	case K_KINS:
	    ins_insert(replaceState);
	    break;

	case K_SELECT:	// end of Select mode mapping - ignore
	    break;

	case K_HELP:	// Help key works like <ESC> <Help>
	case K_F1:
	case K_XF1:
	    stuffcharReadbuff(K_HELP);
	    if (p_im)
		need_start_insertmode = TRUE;
	    goto doESCkey;

#ifdef FEAT_NETBEANS_INTG
	case K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    break;
#endif

	case K_ZERO:	// Insert the previously inserted text.
	case NUL:
	case Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    if (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		goto doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    break;

	case Ctrl_R:	// insert the contents of a register
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    break;

	case Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    break;

	case Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    break;

#ifdef FEAT_RIGHTLEFT
	case Ctrl__:	// switch between languages
	    if (!p_ari)
		goto normalchar;
	    ins_ctrl_();
	    break;
#endif

	case Ctrl_D:	// Make indent one shiftwidth smaller.
#if defined(FEAT_FIND_ID)
	    if (ctrl_x_mode_path_defines())
		goto docomplete;
#endif
	    // FALLTHROUGH

	case Ctrl_T:	// Make indent one shiftwidth greater.
	    if (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		if (has_compl_option(FALSE))
		    goto docomplete;
		break;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    break;

	case K_DEL:	// delete character under the cursor
	case K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    break;

	case K_BS:	// delete character before the cursor
	case Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    break;

	case Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    if (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		count = 0;
		goto doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    break;

	case Ctrl_U:	// delete all inserted text in current line
# ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    if (ctrl_x_mode_function())
		goto docomplete;
# endif
	    did_backspace = ins_bs(c, BACKSPACE_LINE, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    break;

	case K_LEFTMOUSE:   // mouse keys
	case K_LEFTMOUSE_NM:
	case K_LEFTDRAG:
	case K_LEFTRELEASE:
	case K_LEFTRELEASE_NM:
	case K_MOUSEMOVE:
	case K_MIDDLEMOUSE:
	case K_MIDDLEDRAG:
	case K_MIDDLERELEASE:
	case K_RIGHTMOUSE:
	case K_RIGHTDRAG:
	case K_RIGHTRELEASE:
	case K_X1MOUSE:
	case K_X1DRAG:
	case K_X1RELEASE:
	case K_X2MOUSE:
	case K_X2DRAG:
	case K_X2RELEASE:
	    ins_mouse(c);
	    break;

	case K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    break;

	case K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    break;

	case K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_LEFT);
	    break;

	case K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_RIGHT);
	    break;

	case K_PS:
	    bracketed_paste(PASTE_INSERT, FALSE, NULL);
	    if (cmdchar == K_PS)
		// invoked from normal mode, bail out
		goto doESCkey;
	    break;
	case K_PE:
	    // Got K_PE without K_PS, ignore.
	    break;

#ifdef FEAT_GUI_TABLINE
	case K_TABLINE:
	case K_TABMENU:
	    ins_tabline(c);
	    break;
#endif

	case K_IGNORE:	// Something mapped to nothing
	    break;

	case K_COMMAND:		    // <Cmd>command<CR>
	case K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    do_cmdkey_command(c, 0);
#ifdef FEAT_TERMINAL
	    if (term_use_loop())
		// Started a terminal that gets the input, exit Insert mode.
		goto doESCkey;
#endif
	    break;

	case K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    if (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    break;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	case K_F4:
	    if (mod_mask != MOD_MASK_ALT)
		goto normalchar;
	    break;
#endif

#ifdef FEAT_GUI
	case K_VER_SCROLLBAR:
	    ins_scroll();
	    break;

	case K_HOR_SCROLLBAR:
	    ins_horscroll();
	    break;
#endif

	case K_HOME:	// <Home>
	case K_KHOME:
	case K_S_HOME:
	case K_C_HOME:
	    ins_home(c);
	    break;

	case K_END:	// <End>
	case K_KEND:
	case K_S_END:
	case K_C_END:
	    ins_end(c);
	    break;

	case K_LEFT:	// <Left>
	    if (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_left();
	    else
		ins_left();
	    break;

	case K_S_LEFT:	// <S-Left>
	case K_C_LEFT:
	    ins_s_left();
	    break;

	case K_RIGHT:	// <Right>
	    if (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_right();
	    else
		ins_right();
	    break;

	case K_S_RIGHT:	// <S-Right>
	case K_C_RIGHT:
	    ins_s_right();
	    break;

	case K_UP:	// <Up>
	    if (pum_visible())
		goto docomplete;
	    if (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    else
		ins_up(FALSE);
	    break;

	case K_S_UP:	// <S-Up>
	case K_PAGEUP:
	case K_KPAGEUP:
	    if (pum_visible())
		goto docomplete;
	    ins_pageup();
	    break;

	case K_DOWN:	// <Down>
	    if (pum_visible())
		goto docomplete;
	    if (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    else
		ins_down(FALSE);
	    break;

	case K_S_DOWN:	// <S-Down>
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
	    if (pum_visible())
		goto docomplete;
	    ins_pagedown();
	    break;

#ifdef FEAT_DND
	case K_DROP:	// drag-n-drop event
	    ins_drop();
	    break;
#endif

	case K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	case TAB:	// TAB or Complete patterns along path
#if defined(FEAT_FIND_ID)
	    if (ctrl_x_mode_path_patterns())
		goto docomplete;
#endif
	    inserted_space = FALSE;
	    if (ins_tab())
		goto normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    break;

	case K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	case CAR:
	case NL:
#if defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    if (bt_quickfix(curbuf) && c == CAR)
	    {
		if (curwin->w_llist_ref == NULL)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		else				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		break;
	    }
#endif
#ifdef FEAT_CMDWIN
	    if (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		goto doESCkey;
	    }
#endif
#ifdef FEAT_JOB_CHANNEL
	    if (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		if (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    goto doESCkey;
		break;
	    }
#endif
	    if (ins_eol(c) == FAIL && !p_im)
		goto doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    break;

	case Ctrl_K:	    // digraph or keyword completion
	    if (ctrl_x_mode_dictionary())
	    {
		if (has_compl_option(TRUE))
		    goto docomplete;
		break;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    if (c == NUL)
		break;
#endif
	    goto normalchar;

	case Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    break;

	case Ctrl_RSB:	// Tag name completion after ^X
	    if (!ctrl_x_mode_tags())
		goto normalchar;
	    goto docomplete;

	case Ctrl_F:	// File name completion after ^X
	    if (!ctrl_x_mode_files())
		goto normalchar;
	    goto docomplete;

	case 's':	// Spelling completion after ^X
	case Ctrl_S:
	    if (!ctrl_x_mode_spell())
		goto normalchar;
	    goto docomplete;

	case Ctrl_L:	// Whole line completion after ^X
	    if (!ctrl_x_mode_whole_line())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		if (p_im)
		{
		    if (echeck_abbr(Ctrl_L + ABBR_OFF))
			break;
		    goto doESCkey;
		}
		goto normalchar;
	    }
	    // FALLTHROUGH

	case Ctrl_P:	// Do previous/next pattern completion
	case Ctrl_N:
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    if (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_line())
		    && !compl_status_local())
		goto normalchar;

docomplete:
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    if (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    break;

	case Ctrl_Y:	// copy from previous line or scroll down
	case Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    break;

	  default:
#ifdef UNIX
	    if (c == intr_char)		// special interrupt char
		goto do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#if defined(FEAT_EVAL)
	    if (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_insert_char_pre(c);
		char_u *p;

		if (str != NULL)
		{
		    if (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			for (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    if (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    else
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		if (c == NUL)
		    break;
	    }
#endif
#ifdef FEAT_SMARTINDENT
	    // Try to perform smart-indenting.
	    ins_try_si(c);
#endif

	    if (c == ' ')
	    {
		inserted_space = TRUE;
#ifdef FEAT_CINDENT
		if (inindent(0))
		    can_cindent = FALSE;
#endif
		if (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    if (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		insert_special(c, FALSE, FALSE);
#ifdef FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#endif
	    }

	    auto_format(FALSE, TRUE);

#ifdef FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#endif
	    break;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again.
	if (c != K_CURSORHOLD
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// If the cursor was moved we didn't just insert a space
	if (arrow_used)
	    inserted_space = FALSE;

#ifdef FEAT_CINDENT
	if (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    if (in_cinkeys(c, ' ', line_is_white))
	    {
		if (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_expr_indent();
	    }
	}
#endif // FEAT_CINDENT

    }	// for (;;)
    // NOTREACHED
}