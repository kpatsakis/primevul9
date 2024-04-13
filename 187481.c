internal_format(
    int		textwidth,
    int		second_indent,
    int		flags,
    int		format_only,
    int		c) // character to be inserted (can be NUL)
{
    int		cc;
    int		skip_pos;
    int		save_char = NUL;
    int		haveto_redraw = FALSE;
    int		fo_ins_blank = has_format_option(FO_INS_BLANK);
    int		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    int		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    int		fo_white_par = has_format_option(FO_WHITE_PAR);
    int		first_line = TRUE;
    colnr_T	leader_len;
    int		no_leader = FALSE;
    int		do_comments = (flags & INSCHAR_DO_COM);
#ifdef FEAT_LINEBREAK
    int		has_lbr = curwin->w_p_lbr;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    if (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	if (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    while (!got_int)
    {
	int	startcol;		// Cursor column at entry
	int	wantcol;		// column at textwidth border
	int	foundcol;		// column for start of spaces
	int	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	int	orig_col = 0;
	char_u	*saved_text = NULL;
	colnr_T	col;
	colnr_T	end_col;
	int	wcc;			// counter for whitespace chars
	int	did_do_comment = FALSE;

	virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	if (virtcol <= (colnr_T)textwidth)
	    break;

	if (no_leader)
	    do_comments = FALSE;
	else if (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	if (do_comments)
	{
	    char_u *line = ml_get_curline();

	    leader_len = get_leader_len(line, NULL, FALSE, TRUE);
	    if (leader_len == 0 && curbuf->b_p_cin)
	    {
		int		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(line);
		if (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      line + comment_start, NULL, FALSE, TRUE);
		    if (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	else
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	if (leader_len == 0)
	    no_leader = TRUE;
	if (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    break;
	if ((startcol = curwin->w_cursor.col) == 0)
	    break;

	// find column of textwidth border
	coladvance((colnr_T)textwidth);
	wantcol = curwin->w_cursor.col;

	curwin->w_cursor.col = startcol;
	foundcol = 0;
	skip_pos = 0;

	// Find position to break at.
	// Stop at first entered white when 'formatoptions' has 'v'
	while ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    if (curwin->w_cursor.col == startcol && c != NUL)
		cc = c;
	    else
		cc = gchar_cursor();
	    if (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		while (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    if (wcc < 2)
			wcc++;
		}
		if (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    break;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		if (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    continue;

		// Don't break until after the comment leader
		if (curwin->w_cursor.col < leader_len)
		    break;
		if (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    if (curwin->w_cursor.col == 0)
			break;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    if (curwin->w_cursor.col <= leader_len)
			break;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    if (WHITECHAR(cc))
			continue;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		if (curwin->w_cursor.col <= (colnr_T)wantcol)
		    break;
	    }
	    else if ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		int ncc;
		int allow_break;

		// Break after or before a multi-byte character.
		if (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    if (curwin->w_cursor.col < leader_len)
			break;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    if (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			if (curwin->w_cursor.col <= (colnr_T)wantcol)
			    break;
		    }
		    curwin->w_cursor.col = col;
		}

		if (curwin->w_cursor.col == 0)
		    break;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		if (WHITECHAR(cc))
		    continue;		// break with space
		// Don't break until after the comment leader.
		if (curwin->w_cursor.col < leader_len)
		    break;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		if (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		if (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    int ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    if (allow_break)
			break;
		    if (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			if (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    break;
			}

			// Neither cc nor ncc is NUL if we are here, so
			// it's safe to inc_cursor.
			col = curwin->w_cursor.col;

			inc_cursor();
			cc  = ncc;
			ncc = gchar_cursor();
			// handle insert
			ncc = (ncc != NUL) ? ncc : c;

			allow_break =
				(enc_utf8 && utf_allow_break(cc, ncc))
				|| enc_dbcs;

			if (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    break;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    if (curwin->w_cursor.col == 0)
		break;
	    dec_cursor();
	}

	if (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    break;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	if (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	else
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	while ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	if (startcol < 0)
	    startcol = 0;

	if (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strsave(ml_get_cursor());
	    curwin->w_cursor.col = orig_col;
	    if (saved_text == NULL)
		break;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    if (!fo_white_par)
		backspace_until_column(foundcol);
	}
	else
	{
	    // put cursor after pos. to break line
	    if (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_line(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	if (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	if (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	if (first_line)
	{
	    if (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		if (second_indent < 0 && has_format_option(FO_Q_NUMBER))
		    second_indent =
				 get_number_indent(curwin->w_cursor.lnum - 1);
		if (second_indent >= 0)
		{
		    if (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    else
			if (leader_len > 0 && second_indent - leader_len > 0)
		    {
			int i;
			int padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			for (i = 0; i < padding; i++)
			    ins_str((char_u *)" ");
		    }
		    else
		    {
			(void)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_line = FALSE;
	}

	if (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	else
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = (colnr_T)STRLEN(ml_get_curline());
	    if (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	line_breakcheck();
    }

    if (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
#endif
    if (!format_only && haveto_redraw)
    {
	update_topline();
	redraw_curbuf_later(VALID);
    }
}