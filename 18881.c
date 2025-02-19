check_termcode(
    int		max_offset,
    char_u	*buf,
    int		bufsize,
    int		*buflen)
{
    char_u	*tp;
    char_u	*p;
    int		slen = 0;	// init for GCC
    int		modslen;
    int		len;
    int		retval = 0;
    int		offset;
    char_u	key_name[2];
    int		modifiers;
    char_u	*modifiers_start = NULL;
    int		key;
    int		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_KEY_CODE_LEN + 1];
    int		i, j;
    int		idx = 0;
    int		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != NULL);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    if (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    for (offset = 0; offset < max_offset; ++offset)
    {
	if (buf == NULL)
	{
	    if (offset >= typebuf.tb_len)
		break;
	    tp = typebuf.tb_buf + typebuf.tb_off + offset;
	    len = typebuf.tb_len - offset;	// length of the input
	}
	else
	{
	    if (offset >= *buflen)
		break;
	    tp = buf + offset;
	    len = *buflen - offset;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	if (*tp == K_SPECIAL)
	{
	    offset += 2;	// there are always 2 extra characters
	    continue;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	for (p = termleader; *p && *p != i; ++p)
	    ;
	if (*p == NUL)
	    continue;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	if (*tp == ESC && !p_ek && (State & MODE_INSERT))
	    continue;

	tp[len] = NUL;
	key_name[0] = NUL;	// no key name found yet
	key_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    if (*tp == CSI)	    // Special key from GUI
	    {
		if (len < 3)
		    return -1;	    // Shouldn't happen
		slen = 3;
		key_name[0] = tp[1];
		key_name[1] = tp[2];
	    }
	}
	else
#endif // FEAT_GUI
	{
	    int  mouse_index_found = -1;

	    for (idx = 0; idx < tc_len; ++idx)
	    {
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_start = NULL;
		if (cpo_koffset && offset && len < slen)
		    continue;
		if (STRNCMP(termcodes[idx].code, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    int	    looks_like_mouse_start = FALSE;

		    if (len < slen)		// got a partial sequence
			return -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    if (termcodes[idx].name[0] == 'K'
				       && VIM_ISDIGIT(termcodes[idx].name[1]))
		    {
			for (j = idx + 1; j < tc_len; ++j)
			    if (termcodes[j].len == slen &&
				    STRNCMP(termcodes[idx].code,
					    termcodes[j].code, slen) == 0)
			    {
				idx = j;
				break;
			    }
		    }

		    if (slen == 2 && len > 2
			    && termcodes[idx].code[0] == ESC
			    && termcodes[idx].code[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			if (!isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_start = TRUE;
			}
			else if (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    int	    count = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    for (;;)
			    {
				++count;
				(void)getdigits(&nr);
				if (nr >= tp + len)
				    return -1;	// partial sequence
				if (*nr != ';')
				    break;
				++nr;
				if (nr >= tp + len)
				    return -1;	// partial sequence
			    }
			    if (count < 4)
				continue;	// no match
			}
		    }
		    if (looks_like_mouse_start)
		    {
			// Only use it when there is no other match.
			if (mouse_index_found < 0)
			    mouse_index_found = idx;
		    }
		    else
		    {
			key_name[0] = termcodes[idx].name[0];
			key_name[1] = termcodes[idx].name[1];
			break;
		    }
		}

		/*
		 * Check for code with modifier, like xterm uses:
		 * <Esc>[123;*X  (modslen == slen - 3)
		 * <Esc>[@;*X    (matches <Esc>[X and <Esc>[1;9X )
		 * Also <Esc>O*X and <M-O>*X (modslen == slen - 2).
		 * When there is a modifier the * matches a number.
		 * When there is no modifier the ;* or * is omitted.
		 */
		if (termcodes[idx].modlen > 0 && mouse_index_found < 0)
		{
		    int at_code;

		    modslen = termcodes[idx].modlen;
		    if (cpo_koffset && offset && len < modslen)
			continue;
		    at_code = termcodes[idx].code[modslen] == '@';
		    if (STRNCMP(termcodes[idx].code, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			int	    n;

			if (len <= modslen)	// got a partial sequence
			    return -1;		// need to get more chars

			if (tp[modslen] == termcodes[idx].code[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			else if (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    continue;
			else if (at_code && tp[modslen] != '1')
			    // no match for "<Esc>[@" with "<Esc>[1"
			    continue;
			else
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    for (j = slen - 2; j < len && (isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    if (len < j)	// got a partial sequence
				return -1;	// need to get more chars
			    if (tp[j - 1] != termcodes[idx].code[slen - 1])
				continue;	// no match

			    modifiers_start = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((char *)modifiers_start);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			key_name[0] = termcodes[idx].name[0];
			key_name[1] = termcodes[idx].name[1];
			break;
		    }
		}
	    }
	    if (idx == tc_len && mouse_index_found >= 0)
	    {
		key_name[0] = termcodes[mouse_index_found].name[0];
		key_name[1] = termcodes[mouse_index_found].name[1];
	    }
	}

#ifdef FEAT_TERMRESPONSE
	if (key_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
# ifdef FEAT_MOUSE_DEC
	    || key_name[0] == KS_DEC_MOUSE
# endif
# ifdef FEAT_MOUSE_PTERM
	    || key_name[0] == KS_PTERM_MOUSE
# endif
	   )
	{
	    char_u *argp = tp[0] == ESC ? tp + 2 : tp + 1;

	    /*
	     * Check for responses from the terminal starting with {lead}:
	     * "<Esc>[" or CSI followed by [0-9>?]
	     *
	     * - Xterm version string: {lead}>{x};{vers};{y}c
	     *   Also eat other possible responses to t_RV, rxvt returns
	     *   "{lead}?1;2c".
	     *
	     * - Cursor position report: {lead}{row};{col}R
	     *   The final byte must be 'R'. It is used for checking the
	     *   ambiguous-width character state.
	     *
	     * - window position reply: {lead}3;{x};{y}t
	     *
	     * - key with modifiers when modifyOtherKeys is enabled:
	     *	    {lead}27;{modifier};{key}~
	     *	    {lead}{key};{modifier}u
	     */
	    if (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			    || (tp[0] == CSI && len >= 2))
		    && (VIM_ISDIGIT(*argp) || *argp == '>' || *argp == '?'))
	    {
		int resp = handle_csi(tp, len, argp, offset, buf,
					     bufsize, buflen, key_name, &slen);
		if (resp != 0)
		{
# ifdef DEBUG_TERMRESPONSE
		    if (resp == -1)
			LOG_TR(("Not enough characters for CSI sequence"));
# endif
		    return resp;
		}
	    }

	    // Check for fore/background color response from the terminal,
	    // starting} with <Esc>] or OSC
	    else if ((*T_RBG != NUL || *T_RFG != NUL)
			&& ((tp[0] == ESC && len >= 2 && tp[1] == ']')
			    || tp[0] == OSC))
	    {
		if (handle_osc(tp, argp, len, key_name, &slen) == FAIL)
		    return -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    else if ((check_for_codes || rcs_status.tr_progress == STATUS_SENT)
		    && ((tp[0] == ESC && len >= 2 && tp[1] == 'P')
			|| tp[0] == DCS))
	    {
		if (handle_dcs(tp, argp, len, key_name, &slen) == FAIL)
		    return -1;
	    }
	}
#endif

	if (key_name[0] == NUL)
	    continue;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#ifdef FEAT_GUI
	/*
	 * Only in the GUI: Fetch the pointer coordinates of the scroll event
	 * so that we know which window to scroll later.
	 */
	if (gui.in_use
		&& key_name[0] == (int)KS_EXTRA
		&& (key_name[1] == (int)KE_X1MOUSE
		    || key_name[1] == (int)KE_X2MOUSE
		    || key_name[1] == (int)KE_MOUSEMOVE_XY
		    || key_name[1] == (int)KE_MOUSELEFT
		    || key_name[1] == (int)KE_MOUSERIGHT
		    || key_name[1] == (int)KE_MOUSEDOWN
		    || key_name[1] == (int)KE_MOUSEUP))
	{
	    char_u	bytes[6];
	    int		num_bytes = get_bytes_from_buf(tp + slen, bytes, 4);

	    if (num_bytes == -1)	// not enough coordinates
		return -1;
	    mouse_col = 128 * (bytes[0] - ' ' - 1) + bytes[1] - ' ' - 1;
	    mouse_row = 128 * (bytes[2] - ' ' - 1) + bytes[3] - ' ' - 1;
	    slen += num_bytes;
	    // equal to K_MOUSEMOVE
	    if (key_name[1] == (int)KE_MOUSEMOVE_XY)
		key_name[1] = (int)KE_MOUSEMOVE;
	}
	else
#endif
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	if (key_name[0] == KS_MOUSE
#ifdef FEAT_MOUSE_GPM
		|| key_name[0] == KS_GPM_MOUSE
#endif
#ifdef FEAT_MOUSE_JSB
		|| key_name[0] == KS_JSBTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_NET
		|| key_name[0] == KS_NETTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_DEC
		|| key_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
		|| key_name[0] == KS_PTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_URXVT
		|| key_name[0] == KS_URXVT_MOUSE
#endif
		|| key_name[0] == KS_SGR_MOUSE
		|| key_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    if (check_termcode_mouse(tp, &slen, key_name, modifiers_start, idx,
							     &modifiers) == -1)
		return -1;
	}

#ifdef FEAT_GUI
	/*
	 * If using the GUI, then we get menu and scrollbar events.
	 *
	 * A menu event is encoded as K_SPECIAL, KS_MENU, KE_FILLER followed by
	 * four bytes which are to be taken as a pointer to the vimmenu_T
	 * structure.
	 *
	 * A tab line event is encoded as K_SPECIAL KS_TABLINE nr, where "nr"
	 * is one byte with the tab index.
	 *
	 * A scrollbar event is K_SPECIAL, KS_VER_SCROLLBAR, KE_FILLER followed
	 * by one byte representing the scrollbar number, and then four bytes
	 * representing a long_u which is the new value of the scrollbar.
	 *
	 * A horizontal scrollbar event is K_SPECIAL, KS_HOR_SCROLLBAR,
	 * KE_FILLER followed by four bytes representing a long_u which is the
	 * new value of the scrollbar.
	 */
# ifdef FEAT_MENU
	else if (key_name[0] == (int)KS_MENU)
	{
	    long_u	val;
	    int		num_bytes = get_long_from_buf(tp + slen, &val);

	    if (num_bytes == -1)
		return -1;
	    current_menu = (vimmenu_T *)val;
	    slen += num_bytes;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    if (check_menu_pointer(root_menu, current_menu) == FAIL)
	    {
		key_name[0] = KS_EXTRA;
		key_name[1] = (int)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	else if (key_name[0] == (int)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    int		num_bytes = get_bytes_from_buf(tp + slen, bytes, 1);

	    if (num_bytes == -1)
		return -1;
	    current_tab = (int)bytes[0];
	    if (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += num_bytes;
	}
	else if (key_name[0] == (int)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    int		num_bytes = get_bytes_from_buf(tp + slen, bytes, 2);

	    if (num_bytes == -1)
		return -1;
	    current_tab = (int)bytes[0];
	    current_tabmenu = (int)bytes[1];
	    slen += num_bytes;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	else if (key_name[0] == (int)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	bytes[6];
	    int		num_bytes;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    for (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		num_bytes = get_bytes_from_buf(tp + j, bytes, 1);
		if (num_bytes == -1)
		    break;
		if (i == 0)
		    current_scrollbar = (int)bytes[0];
		else if (current_scrollbar != (int)bytes[0])
		    break;
		j += num_bytes;
		num_bytes = get_long_from_buf(tp + j, &val);
		if (num_bytes == -1)
		    break;
		scrollbar_value = val;
		j += num_bytes;
		slen = j;
	    }
	    if (i == 0)		// not enough characters to make one
		return -1;
	}
	else if (key_name[0] == (int)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    int		num_bytes;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    for (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		num_bytes = get_long_from_buf(tp + j, &val);
		if (num_bytes == -1)
		    break;
		scrollbar_value = val;
		j += num_bytes;
		slen = j;
	    }
	    if (i == 0)		// not enough characters to make one
		return -1;
	}
# endif // !USE_ON_FLY_SCROLL
#endif // FEAT_GUI

#if (defined(UNIX) || defined(VMS))
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	if (key_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.in_use
# endif
	    )
	{
	    if (key_name[1] == KE_FOCUSGAINED)
	    {
		if (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		key_name[1] = (int)KE_IGNORE;
	    }
	    else if (key_name[1] == KE_FOCUSLOST)
	    {
		if (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		key_name[1] = (int)KE_IGNORE;
	    }
	}
#endif

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	key = handle_x_keys(TERMCAP2KEY(key_name[0], key_name[1]));

	/*
	 * Add any modifier codes to our string.
	 */
	new_slen = modifiers2keycode(modifiers, &key, string);

	// Finally, add the special key code to our string
	key_name[0] = KEY2TERMCAP0(key);
	key_name[1] = KEY2TERMCAP1(key);
	if (key_name[0] == KS_KEY)
	{
	    // from ":set <M-b>=xx"
	    if (has_mbyte)
		new_slen += (*mb_char2bytes)(key_name[1], string + new_slen);
	    else
		string[new_slen++] = key_name[1];
	}
	else if (new_slen == 0 && key_name[0] == KS_EXTRA
						  && key_name[1] == KE_IGNORE)
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	else
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = key_name[0];
	    string[new_slen++] = key_name[1];
	}
	if (put_string_in_typebuf(offset, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    return -1;
	return retval == 0 ? (len + new_slen - slen + offset) : retval;
    }

#ifdef FEAT_TERMRESPONSE
    LOG_TR(("normal character"));
#endif

    return 0;			    // no match found
}