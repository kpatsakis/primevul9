fold_line(
    win_T	*wp,
    long	fold_count,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    int		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*top, *bot;
    linenr_T	lnume = lnum + fold_count - 1;
    int		len;
    char_u	*text;
    int		fdc;
    int		col;
    int		txtcol;
    int		off = (int)(current_ScreenLine - ScreenLines);
    int		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    col = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
#ifdef FEAT_CMDWIN
    if (cmdwin_type != 0 && wp == curwin)
    {
	ScreenLines[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	if (enc_utf8)
	    ScreenLinesUC[off] = 0;
	++col;
    }
#endif

#ifdef FEAT_RIGHTLEFT
# define RL_MEMSET(p, v, l) \
    do { \
	if (wp->w_p_rl) \
	    for (ri = 0; ri < l; ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 else \
	    for (ri = 0; ri < l; ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } while (0)
#else
# define RL_MEMSET(p, v, l) \
    do { \
	for (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } while (0)
#endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    if (fdc > 0)
    {
	char_u	*p;
	int	i;
	int	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	for (i = 0; i < fdc; i++)
	{
	    int		ch;

	    if (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    else
		ch = *p++;
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    else
#endif
		idx = off + col + i;
	    if (enc_utf8)
	    {
		if (ch >= 0x80)
		{
		    ScreenLinesUC[idx] = ch;
		    ScreenLinesC[0][idx] = 0;
		    ScreenLines[idx] = 0x80;
		}
		else
		{
		    ScreenLines[idx] = ch;
		    ScreenLinesUC[idx] = 0;
		}
	    }
	    else
		ScreenLines[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

#ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    if (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	if (len > 0)
	{
	    if (len > 2)
		len = 2;
# ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    else
# endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
#endif

    // 3. Add the 'number' or 'relativenumber' column
    if (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	if (len > 0)
	{
	    int	    w = number_width(wp);
	    long    num;
	    char    *fmt = "%*ld ";

	    if (len > w + 1)
		len = w + 1;

	    if (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		num = (long)lnum;
	    else
	    {
		// 'relativenumber', don't use negative numbers
		num = labs((long)get_cursor_rel_lnum(wp, lnum));
		if (num == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    num = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    sprintf((char *)buf, fmt, w, num);
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    else
#endif
		copy_text_attr(off + col, buf, len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    text = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = col;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fill_fold".
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    col = text_to_screenline(wp, text, col);

    // Fill the rest of the line with the fold filler
#ifdef FEAT_RIGHTLEFT
    if (wp->w_p_rl)
	col -= txtcol;
#endif
    while (col < wp->w_width
#ifdef FEAT_RIGHTLEFT
		    - (wp->w_p_rl ? txtcol : 0)
#endif
	    )
    {
	if (enc_utf8)
	{
	    if (fill_fold >= 0x80)
	    {
		ScreenLinesUC[off + col] = fill_fold;
		ScreenLinesC[0][off + col] = 0;
		ScreenLines[off + col] = 0x80; // avoid storing zero
	    }
	    else
	    {
		ScreenLinesUC[off + col] = 0;
		ScreenLines[off + col] = fill_fold;
	    }
	    col++;
	}
	else
	    ScreenLines[off + col++] = fill_fold;
    }

    if (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    if (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	if (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    top = &curwin->w_cursor;
	    bot = &VIsual;
	}
	else
	{
	    // Visual is before curwin->w_cursor
	    top = &VIsual;
	    bot = &curwin->w_cursor;
	}
	if (lnum >= top->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > top->lnum
			    || (lnum == top->lnum
				&& top->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
		>= (colnr_T)STRLEN(ml_get_buf(wp->w_buffer, lnume, FALSE)))))))
	{
	    if (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		if (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    if (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    else
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (int)wp->w_old_cursor_fcol);
		}
	    }
	    else
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

#ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    if (wp->w_p_cc_cols)
    {
	int i = 0;
	int j = wp->w_p_cc_cols[i];
	int old_txtcol = txtcol;

	while (j > -1)
	{
	    txtcol += j;
	    if (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    else
		txtcol -= wp->w_leftcol;
	    if (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    if (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	if (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	else
	    txtcol -= wp->w_leftcol;
	if (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
#endif

    screen_line(row + W_WINROW(wp), wp->w_wincol, (int)wp->w_width,
						     (int)wp->w_width, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    if (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_row = row;
	curwin->w_cline_height = 1;
	curwin->w_cline_folded = TRUE;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }

# ifdef FEAT_CONCEAL
    // When the line was not folded w_wrow may have been set, recompute it.
    if (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_line(wp))
	curs_columns(TRUE);
# endif
}