format_lines(
    linenr_T	line_count,
    int		avoid_fex)		// don't use 'formatexpr'
{
    int		max_len;
    int		is_not_par;		// current line not part of parag.
    int		next_is_not_par;	// next line not part of paragraph
    int		is_end_par;		// at end of paragraph
    int		prev_is_end_par = FALSE;// prev. line not part of parag.
    int		next_is_start_par = FALSE;
    int		leader_len = 0;		// leader len of current line
    int		next_leader_len;	// leader len of next line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    char_u	*next_leader_flags = NULL; // flags for leader of next line
    int		do_comments;		// format comments
    int		do_comments_list = 0;	// format comments with 'n' or '2'
    int		advance = TRUE;
    int		second_indent = -1;	// indent for second line (comment
					// aware)
    int		do_second_indent;
    int		do_number_indent;
    int		do_trail_white;
    int		first_par_line = TRUE;
    int		smd_save;
    long	count;
    int		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_line = curwin->w_cursor.lnum;
    int		force_format = FALSE;
    int		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2' and '1' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_number_indent = has_format_option(FO_Q_NUMBER);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    if (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    else
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    if (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    for (count = line_count; count != 0 && !got_int; --count)
    {
	// Advance to next paragraph.
	if (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	if (count == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = NULL;
	}
	else
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    if (do_number_indent)
		next_is_start_par =
			   (get_number_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	if (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	if (is_not_par)
	{
	    if (line_count < 0)
		break;
	}
	else
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    if (first_par_line
		    && (do_second_indent || do_number_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		if (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    if (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    else
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		else if (do_number_indent)
		{
		    if (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
		    }
		    else
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    if (curwin->w_cursor.lnum >= curbuf->b_ml.ml_line_count
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		if (next_leader_flags == NULL
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    if (is_end_par || force_format)
	    {
		if (need_set_indent)
		{
		    int		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    if (curwin->w_cursor.lnum == first_line)
			indent = get_indent();
		    else if (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    else
		    {
			if (cindent_on())
			{
			    indent =
# ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_expr_indent() :
# endif
				 get_c_indent();
			}
			else
			    indent = get_indent();
		    }
		    (void)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		while (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_INSERT;	// for open_line()
		smd_save = p_smd;
		p_smd = FALSE;
		insertchar(NUL, INSCHAR_FORMAT
			+ (do_comments ? INSCHAR_DO_COM : 0)
			+ (do_comments && do_comments_list
						       ? INSCHAR_COM_LIST : 0)
			+ (avoid_fex ? INSCHAR_NO_FEX : 0), second_indent);
		State = old_State;
		p_smd = smd_save;
		second_indent = -1;
		// at end of par.: need to set indent of next par.
		need_set_indent = is_end_par;
		if (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    if (line_count < 0)
			break;
		    first_par_line = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    if (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		if (line_count < 0 && u_save_cursor() == FAIL)
		    break;
		if (next_leader_len > 0)
		{
		    (void)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		else if (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    int indent = getwhitecols_curline();

		    if (indent > 0)
		    {
			(void)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					       (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		if (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    break;
		}
		first_par_line = FALSE;
		// If the line is getting long, format it next time
		if (STRLEN(ml_get_curline()) > (size_t)max_len)
		    force_format = TRUE;
		else
		    force_format = FALSE;
	    }
	}
	line_breakcheck();
    }
}