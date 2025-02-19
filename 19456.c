do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    int		c,		// K_LEFTMOUSE, etc
    int		dir,		// Direction to 'put' if necessary
    long	count,
    int		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    static int	do_always = FALSE;	// ignore 'mouse' setting next time
    static int	got_click = FALSE;	// got a click some time back

    int		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    int		is_click = FALSE; // If FALSE it's a drag or release event
    int		is_drag = FALSE;  // If TRUE it's a drag event
    int		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    int		moved;		// Has cursor moved?
    int		in_status_line;	// mouse in status line
    static int	in_tab_line = FALSE; // mouse clicked in tab line
    int		in_sep_line;	// mouse in vertical separator line
    int		c1, c2;
#if defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    static pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    int		diff;
    int		old_active = VIsual_active;
    int		old_mode = VIsual_mode;
    int		regname;

#if defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    if (do_always)
	do_always = FALSE;
    else
#ifdef FEAT_GUI
	if (!gui.in_use)
#endif
	{
	    if (VIsual_active)
	    {
		if (!mouse_has(MOUSE_VISUAL))
		    return FALSE;
	    }
	    else if (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		return FALSE;
	}

    for (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	if (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    if (!KeyStuffed && vpeekc() != NUL)
	    {
		int nc;
		int save_mouse_row = mouse_row;
		int save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		if (c == nc)
		    continue;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	break;
    }

    if (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	if (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	return FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    if (!is_drag && drag_status_line)
    {
	drag_status_line = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    if (!is_drag && drag_sep_line)
    {
	drag_sep_line = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    if (is_click)
	got_click = TRUE;
    else
    {
	if (!got_click)			// didn't get click, ignore
	    return FALSE;
	if (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    if (in_tab_line)
	    {
		in_tab_line = FALSE;
		return FALSE;
	    }
	}
    }

    // CTRL right mouse button does CTRL-T
    if (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_RIGHT)
    {
	if (State & MODE_INSERT)
	    stuffcharReadbuff(Ctrl_O);
	if (count > 1)
	    stuffnumReadbuff(count);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	return FALSE;
    }

    // CTRL only works with left mouse button
    if ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_LEFT)
	return FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    if ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_MIDDLE)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_LEFT)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_RIGHT)
	    )
	return FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    if (!is_click && which_button == MOUSE_MIDDLE)
	return FALSE;

    if (oap != NULL)
	regname = oap->regname;
    else
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    if (which_button == MOUSE_MIDDLE)
    {
	if (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    if (oap != NULL && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		return FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    if (VIsual_active)
	    {
		if (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		else
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		return FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	else if ((State & MODE_INSERT) == 0)
	    return FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	if ((State & MODE_INSERT) || !mouse_has(MOUSE_NORMAL))
	{
	    if (regname == '.')
		insert_reg(regname, TRUE);
	    else
	    {
#ifdef FEAT_CLIPBOARD
		if (clip_star.available && regname == 0)
		    regname = '*';
#endif
		if ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    insert_reg(regname, TRUE);
		else
		{
		    do_put(regname, NULL, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    return FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    if (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    if (TabPageIdxs != NULL)  // only when initialized
    {
	// Check for clicking in the tab page line.
	if (mouse_row == 0 && firstwin->w_winrow > 0)
	{
	    if (is_drag)
	    {
		if (in_tab_line)
		{
		    c1 = TabPageIdxs[mouse_col];
		    tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
								    ? c1 - 1 : c1);
		}
		return FALSE;
	    }

	    // click in a tab selects that tab page
	    if (is_click
# ifdef FEAT_CMDWIN
		    && cmdwin_type == 0
# endif
		    && mouse_col < Columns)
	    {
		in_tab_line = TRUE;
		c1 = TabPageIdxs[mouse_col];
		if (c1 >= 0)
		{
		    if ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
		    {
			// double click opens new page
			end_visual_mode_keep_button();
			tabpage_new();
			tabpage_move(c1 == 0 ? 9999 : c1 - 1);
		    }
		    else
		    {
			// Go to specified tab page, or next one if not clicking
			// on a label.
			goto_tabpage(c1);

			// It's like clicking on the status line of a window.
			if (curwin != old_curwin)
			    end_visual_mode_keep_button();
		    }
		}
		else
		{
		    tabpage_T	*tp;

		    // Close the current or specified tab page.
		    if (c1 == -999)
			tp = curtab;
		    else
			tp = find_tabpage(-c1);
		    if (tp == curtab)
		    {
			if (first_tabpage->tp_next != NULL)
			    tabpage_close(FALSE);
		    }
		    else if (tp != NULL)
			tabpage_close_other(tp, FALSE);
		}
	    }
	    return TRUE;
	}
	else if (is_drag && in_tab_line)
	{
	    c1 = TabPageIdxs[mouse_col];
	    tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	    return FALSE;
	}
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    if (mouse_model_popup())
    {
	if (which_button == MOUSE_RIGHT
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    if (gui.in_use)
	    {
#  if defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		if (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    return FALSE;
#  endif
#  if defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		if (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    return FALSE;
#  endif
	    }
# endif
# if defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    else
# endif
# if defined(FEAT_TERM_POPUP_MENU)
	    if (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		return FALSE;
#endif

	    jump_flags = 0;
	    if (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		if (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    if (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    else if (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    else
		    {
			if ((LT_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			else if (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol);
			    getvcol(curwin, &m_pos, NULL, &m_pos.col, NULL);
			    if (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		else
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    if (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, NULL, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    return (jump_flags & CURSOR_MOVED) != 0;
#else
	    return FALSE;
#endif
	}
	if (which_button == MOUSE_LEFT
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_RIGHT;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    if ((State & (MODE_NORMAL | MODE_INSERT))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	if (which_button == MOUSE_LEFT)
	{
	    if (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		if (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    else if (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	else if (which_button == MOUSE_RIGHT)
	{
	    if (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		if (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		else
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    if (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    if (!is_drag && oap != NULL && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    if (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == NULL ? NULL : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    if (jump_flags & MOUSE_WINBAR)
	return FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_line = (jump_flags & IN_STATUS_LINE);
    in_sep_line = (jump_flags & IN_SEP_LINE);

#ifdef FEAT_NETBEANS_INTG
    if (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_LINE | IN_SEP_LINE)))
    {
	int key = KEY2TERMCAP1(c);

	if (key == (int)KE_LEFTRELEASE || key == (int)KE_MIDDLERELEASE
					       || key == (int)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    if (curwin != old_curwin && oap != NULL && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    if (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_LEFT)
    {
	// open or close a fold at this line
	if (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	else
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	if (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#if defined(FEAT_CLIPBOARD) && defined(FEAT_CMDWIN)
    if ((jump_flags & IN_OTHER_WIN) && !VIsual_active && clip_star.available)
    {
	clip_modeless(which_button, is_click, is_drag);
	return FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    if (VIsual_active && is_drag && get_scrolloff_value())
    {
	// In the very first line, allow scrolling one line
	if (mouse_row == 0)
	    mouse_dragging = 2;
	else
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    if (is_drag && mouse_row < 0 && !in_status_line)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    if (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       if (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	if (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual, &leftcol, &rightcol);
	    if (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    else
		end_visual.col = rightcol;
	    if (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	else
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    if (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    else if (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    else
	    {
		// In the same line, compare column number
		if (end_visual.lnum == start_visual.lnum)
		{
		    if (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    else
			VIsual = end_visual;
		}

		// In different lines, compare line number
		else
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    if (diff > 0)		// closest to end
			VIsual = start_visual;
		    else if (diff < 0)	// closest to start
			VIsual = end_visual;
		    else			// in the middle line
		    {
			if (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			else
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    else if ((State & MODE_INSERT) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    if (which_button == MOUSE_MIDDLE)
    {
#ifdef FEAT_CLIPBOARD
	if (clip_star.available && regname == 0)
	    regname = '*';
#endif
	if (yank_register_mline(regname))
	{
	    if (mouse_past_bottom)
		dir = FORWARD;
	}
	else if (mouse_past_eol)
	    dir = FORWARD;

	if (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	else
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, count, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	if (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, NULL, dir, count, fixindent | PUT_CURSEND);
    }

#if defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    else if (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	if (curwin->w_llist_ref == NULL)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	else					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    else if ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	if (State & MODE_INSERT)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    else if ((mod_mask & MOD_MASK_SHIFT))
    {
	if ((State & MODE_INSERT) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	if (which_button == MOUSE_LEFT)
	    stuffcharReadbuff('*');
	else	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    else if (in_status_line)
    {
#ifdef FEAT_MOUSESHAPE
	if ((is_drag || is_click) && !drag_status_line)
	{
	    drag_status_line = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    else if (in_sep_line)
    {
#ifdef FEAT_MOUSESHAPE
	if ((is_drag || is_click) && !drag_sep_line)
	{
	    drag_sep_line = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    else if ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_INSERT))
	     && mouse_has(MOUSE_VISUAL))
    {
	if (is_click || !VIsual_active)
	{
	    if (VIsual_active)
		orig_cursor = VIsual;
	    else
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_active = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_start_select('o');
		setmouse();
	    }
	    if ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		if (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		else
		    VIsual_mode = 'v';
	    }
	    else if ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    else if ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	if ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = NULL;
	    int		gc;

	    if (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		while (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		if (oap != NULL)
		    oap->motion_type = MCHAR;
		if (oap != NULL
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != NULL)
		{
		    curwin->w_cursor = *pos;
		    if (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    else if (*p_sel == 'e')
		    {
			if (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			else
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    if (pos == NULL && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		if (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		else
		{
		    find_start_of_word(&VIsual);
		    if (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = TRUE;
	}
	if (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    else if (VIsual_active && !old_active)
    {
	if (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	else
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    if ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && msg_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    return moved;
}