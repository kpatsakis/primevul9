get_c_indent(void)
{
    pos_T	cur_curpos;
    int		amount;
    int		scope_amount;
    int		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = NULL;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    int		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    int		lookfor;
    int		whilelevel;
    linenr_T	lnum;
    int		n;
    int		iscase;
    int		lookfor_break;
    int		lookfor_cpp_namespace = FALSE;
    int		cont_amount = 0;    // amount for continuation line
    int		original_line_islabel;
    int		added_to_amount = 0;
    int		js_cur_has_key = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    int		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    if (cur_curpos.lnum == 1)
	return 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    if (linecopy == NULL)
	return 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    if ((State & MODE_INSERT)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_line_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    if (comment_pos != NULL)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    if (trypos != NULL && (comment_pos == NULL
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	goto laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    if (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	if (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    goto theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    if (original_line_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	goto theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    if (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_line_comment(); // XXX
	if (trypos == NULL && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    if (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	if (trypos  != NULL)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, NULL, NULL);
	    amount = col;
	    goto theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    if (!cin_iscomment(theline) && comment_pos != NULL) // XXX
    {
	int	lead_start_len = 2;
	int	lead_middle_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_middle[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	char_u	*p;
	int	start_align = 0;
	int	start_off = 0;
	int	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, NULL, NULL);
	amount = col;
	*lead_start = NUL;
	*lead_middle = NUL;

	p = curbuf->b_p_com;
	while (*p != NUL)
	{
	    int	align = 0;
	    int	off = 0;
	    int what = 0;

	    while (*p != NUL && *p != ':')
	    {
		if (*p == COM_START || *p == COM_END || *p == COM_MIDDLE)
		    what = *p++;
		else if (*p == COM_LEFT || *p == COM_RIGHT)
		    align = *p++;
		else if (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		else
		    ++p;
	    }

	    if (*p == ':')
		++p;
	    (void)copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    if (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = (int)STRLEN(lead_start);
		start_off = off;
		start_align = align;
	    }
	    else if (what == COM_MIDDLE)
	    {
		STRCPY(lead_middle, lead_end);
		lead_middle_len = (int)STRLEN(lead_middle);
	    }
	    else if (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		if (STRNCMP(theline, lead_middle, lead_middle_len) == 0
			&& STRNCMP(theline, lead_end, STRLEN(lead_end)) != 0)
		{
		    done = TRUE;
		    if (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			if (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			else if (STRNCMP(look, lead_middle,
							lead_middle_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    break;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			else if (STRNCMP(ml_get(comment_pos->lnum) + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    continue;
		    }
		    if (start_off != 0)
			amount += start_off;
		    else if (start_align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    break;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		if (STRNCMP(theline, lead_middle, lead_middle_len) != 0
			&& STRNCMP(theline, lead_end, STRLEN(lead_end)) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    if (off != 0)
			amount += off;
		    else if (align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    done = TRUE;
		    break;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	if (done)
	    ;
	else if (theline[0] == '*')
	    amount += 1;
	else
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    for (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		if (linewhite(lnum))		    // skip blank lines
		    continue;
		amount = get_indent_lnum(lnum);	    // XXX
		break;
	    }
	    if (amount == -1)			    // use the comment opener
	    {
		if (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    if (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, NULL, NULL);
		amount = col;
		if (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	goto theend;
    }

    // Are we looking at a ']' that has a match?
    if (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != NULL)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	goto theend;
    }

    // Are we inside parentheses or braces?  XXX
    if (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != NULL
	    || trypos != NULL)
    {
      if (trypos != NULL && tryposBrace != NULL)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  if (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = NULL;
	  else
	      tryposBrace = NULL;
      }

      if (trypos != NULL)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	if (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	else
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    for (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		if (cin_nocode(l))		// skip comment lines
		    continue;
		if (cin_ispreproc_cont(&l, &lnum, &amount))
		    continue;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		if ((trypos = ind_find_start_CORS(NULL)) != NULL)
		{
		    lnum = trypos->lnum + 1;
		    continue;
		}

		// XXX
		if ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != NULL
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
			amount = get_indent_lnum(lnum);	// XXX

			if (theline[0] == ')')
			{
			    if (our_paren_pos.lnum != lnum
						       && cur_amount > amount)
				cur_amount = amount;
			    amount = -1;
			}
		    break;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	if (amount == -1)
	{
	    int	    ignore_paren_col = 0;
	    int	    is_if_for_while = 0;

	    if (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *line;

		trypos = &our_paren_pos;
		do {
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} while (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		line = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(line, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    if (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *line;
		int	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		line = ml_get_curline();
		look_col = (int)(look - line);
		curwin->w_cursor.col = look_col + 1;
		if ((trypos = findmatchlimit(NULL, ')', 0,
						      curbuf->b_ind_maxparen))
								      != NULL
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    if (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_col == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		if (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    if (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"(", NULL))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			for (col = 0; col < our_paren_pos.col; ++col)
			{
			    switch (l[col])
			    {
				case '(':
				case '{': ++n;
					  break;

				case ')':
				case '}': if (n > 1)
					      --n;
					  break;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    else if (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    else
		    {
			col = our_paren_pos.col + 1;
			while (VIM_ISWHITE(l[col]))
			    col++;
			if (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			else
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		if (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, NULL, NULL);
		    if (cur_amount > (int)col)
			cur_amount = col;
		}
	    }

	    if (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    else if ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		if (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    else
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		while ((int)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    switch (*ml_get_pos(&our_paren_pos))
		    {
			case '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  break;
			case ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  break;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		if (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		else
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    if (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != NULL)
			amount += curbuf->b_ind_unclosed2;
		    else
		    {
			if (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			else
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		if (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	if (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      else
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	start = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(start);
	if (*look == '{')
	{
	    getvcol(curwin, trypos, &col, NULL, NULL);
	    amount = col;
	    if (*start == '{')
		start_brace = BRACE_IN_COL0;
	    else
		start_brace = BRACE_AT_START;
	}
	else
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    if (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != NULL)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			ldfd) {
	    //		    }
	    if ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    else if (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    else
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	if (curbuf->b_ind_js)
	    js_cur_has_key = cin_has_js_key(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	if (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	else
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    if (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    else if (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    if (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		if (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    goto theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    if (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_left_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    else if (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    else
	    {
		if (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    if (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    else if (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		else
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    if (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    if (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    else if (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    else
	    {
		if (curbuf->b_ind_case_break && cin_isbreak(theline))
		    // break; ...
		    lookfor_break = TRUE;

		lookfor = LOOKFOR_INITIAL;
		// b_ind_level from start of block
		amount += curbuf->b_ind_level;
	    }
	    scope_amount = amount;
	    whilelevel = 0;

	    // Search backwards.  If we find something we recognize, line up
	    // with that.
	    //
	    // If we're looking at an open brace, indent
	    // the usual amount relative to the conditional
	    // that opens the block.
	    curwin->w_cursor = cur_curpos;
	    for (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		if (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    if (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			if (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    if (cont_amount > 0)
				amount = cont_amount;
			    else if (!curbuf->b_ind_js)
				amount += ind_continuation;
			    break;
			}

			l = ml_get_curline();

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(NULL);
			if (trypos != NULL)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    continue;
			}

			// Skip preprocessor directives and blank lines.
			if (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    continue;

			if (cin_nocode(l))
			    continue;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			if (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    if (terminated == ',')
				break;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    if (terminated != ';' && cin_isinit())
				break;

			    // nothing useful found
			    if (terminated == 0 || terminated == '{')
				continue;
			}

			if (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = NULL;
			    if (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    if (trypos == NULL && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    if (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			if (cont_amount > 0)
			    amount = cont_amount;
			else
			    amount += ind_continuation;
		    }
		    else if (lookfor == LOOKFOR_UNTERM)
		    {
			if (cont_amount > 0)
			    amount = cont_amount;
			else
			    amount += ind_continuation;
		    }
		    else
		    {
			if (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    if (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			if (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    if (curwin->w_cursor.lnum == ourscope)
				continue;

			    if (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				break;

			    l = ml_get_curline();

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(NULL);
			    if (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }

			    // Skip preprocessor directives and blank lines.
			    if (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				continue;

			    // Finally the actual check for "namespace".
			    if (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				break;
			    }
			    else if (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				break;
			    }

			    if (cin_nocode(l))
				continue;
			}
		    }
		    break;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		if ((trypos = ind_find_start_CORS(&raw_string_start)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    continue;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		if (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    if (lookfor == LOOKFOR_CPP_BASECLASS)
			break;

		    // When looking for a "do" we are not interested in
		    // labels.
		    if (whilelevel > 0)
			continue;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    if (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			if (cont_amount > 0)
			    amount = cont_amount;
			else
			    amount += ind_continuation;
			break;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    if (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			if ((trypos = find_start_brace()) == NULL
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    break;
			}
			continue;
		    }

		    n = get_indent_nolabel(curwin->w_cursor.lnum);  // XXX

		    //	 case xx: if (cond)	    <- line up with this if
		    //		      y = y + 1;
		    // ->	  s = 99;
		    //
		    //	 case xx:
		    //	     if (cond)		<- line up with this line
		    //		 y = y + 1;
		    // ->    s = 99;
		    if (lookfor == LOOKFOR_TERM)
		    {
			if (n)
			    amount = n;

			if (!lookfor_break)
			    break;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    if (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			if (l != NULL && cin_is_cinword(l))
			{
			    if (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    else
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			break;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_code
					: curbuf->b_ind_scopedecl_code);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    continue;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		if (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    if (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != NULL)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    continue;
		}

		// Ignore jump labels with nothing after them.
		if (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    if (l == NULL || cin_nocode(l))
			continue;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		if (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    continue;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		if (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		if (n)
		{
		    if (lookfor == LOOKFOR_UNTERM)
		    {
			if (cont_amount > 0)
			    amount = cont_amount;
			else
			    amount += ind_continuation;
		    }
		    else if (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			continue;
		    }
		    else
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    break;
		}
		else if (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    if (cin_isterminated(l, TRUE, FALSE))
			break;
		    else
			continue;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//	  here
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		if (js_cur_has_key)
		{
		    js_cur_has_key = 0; // only check the first line
		    if (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_KEY;
		    }
		}
		if (lookfor == LOOKFOR_JS_KEY && cin_has_js_key(l))
		{
		    amount = get_indent();
		    break;
		}
		if (lookfor == LOOKFOR_COMMA)
		{
		    if (tryposBrace != NULL && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			break;
		    if (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			break;
		    else
		    {
			amount = get_indent();
			if (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    break;
		    }
		}

		if (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    if (lookfor != LOOKFOR_ENUM_OR_INIT &&
			    (*skipwhite(l) == '[' || l[STRLEN(l) - 1] == '['))
			amount += ind_continuation;
		    // if we're in the middle of a paren thing,
		    // go back to the line that starts it so
		    // we can get the right prevailing indent
		    //	   if ( foo &&
		    //		    bar )

		    // Position the cursor over the rightmost paren, so that
		    // matching it will take us back to the start of the line.
		    // Ignore a match before the start of the block.
		    (void)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    if (trypos != NULL && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = NULL;

		    // If we are looking for ',', we also look for matching
		    // braces.
		    if (trypos == NULL && terminated == ','
					      && find_last_paren(l, '{', '}'))
			trypos = find_start_brace();

		    if (trypos != NULL)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			if (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    continue;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    if (terminated == ',')
		    {
			while (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    if (*l == NUL || l[STRLEN(l) - 1] != '\\')
				break;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    if (curbuf->b_ind_js)
			cur_amount = get_indent();
		    else
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    if (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			if (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			if (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    continue;
			}
			break;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "   } else".
		    if (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			if (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    if (cont_amount > 0)
				amount = cont_amount;
			    else
				amount += ind_continuation;
			    break;
			}

			// If this is just above the line we are indenting, we
			// are finished.
			//	    while (not)
			// ->		here;
			// Otherwise this indent can be used when the line
			// before this is terminated.
			//	yyy;
			//	if (stat)
			//	    while (not)
			//		xxx;
			// ->	here;
			amount = cur_amount;
			if (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			if (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    break;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			if (cin_isdo(l))
			{
			    if (whilelevel == 0)
				break;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			if (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if () {".
			    if (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    if ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				break;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    else
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			if (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    if (terminated == ',')
				amount += ind_continuation;
			    break;
			}

			if (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    if (terminated == ',')
			    {
				if (curbuf->b_ind_cpp_baseclass == 0)
				    break;

				lookfor = LOOKFOR_CPP_BASECLASS;
				continue;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    if (amount > cur_amount)
				amount = cur_amount;
			}
			else
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (int)STRLEN(l);
			    if (terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				break;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    if (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				if (curbuf->b_ind_js)
				{
				    // Search for a line ending in a comma
				    // and line up with the line below it
				    // (could be the current line).
				    // some = [
				    //     1,     <- line up here
				    //     2,
				    // some = [
				    //     3 +    <- line up here
				    //       4 *
				    //        5,
				    //     6,
				    if (cin_iscomment(skipwhite(l)))
					break;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    if (trypos != NULL)
				    {
					if (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    break;
					}
					ourscope = trypos->lnum;
				    }
				}
				else
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    else
			    {
				if (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				if (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_KEY
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		else if (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    if (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			if (cont_amount > 0)
			    amount = cont_amount;
			else
			    amount += ind_continuation;
			break;
		    }

		    if (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			if (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		else
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    if (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			continue;
		    }

		    // Handle "do {" line.
		    if (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			if (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    continue;
			}
		    }

		    // Found a terminated line above an unterminated line. Add
		    // the amount for a continuation line.
		    //	 x = 1;
		    //	 y = foo +
		    // ->	here;
		    // or
		    //	 int x = 1;
		    //	 int foo,
		    // ->	here;
		    if (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			if (cont_amount > 0)
			    amount = cont_amount;
			else
			    amount += ind_continuation;
			break;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    if (lookfor == LOOKFOR_TERM)
		    {
			if (!lookfor_break && whilelevel == 0)
			    break;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    else
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			if (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != NULL)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    if (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				continue;
			    }
			}

			// When aligning with the case statement, don't align
			// with a statement after it.
			//  case 1: {   <-- don't use this { position
			//	stat;
			//  }
			//  case 2:
			//	stat;
			// }
			iscase = (curbuf->b_ind_keep_case_label
						     && cin_iscase(l, FALSE));

			// Get indent and pointer to text for current line,
			// ignoring any jump label.
			amount = skip_label(curwin->w_cursor.lnum, &l);

			if (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			if (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			if (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    if ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				break;
			    continue;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			if (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != NULL)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    if (*l == '}' || !cin_iselse(l))
				goto term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      if (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      if (curbuf->b_ind_jump_label > 0 && original_line_islabel)
	  amount -= curbuf->b_ind_jump_label;

      goto theend;
    }

    // ok -- we're not inside any sort of structure at all!
    //
    // This means we're at the top level, and everything should
    // basically just match where the previous line is, except
    // for the lines immediately following a function declaration,
    // which are K&R-style parameters and need to be indented.
    //
    // if our line starts with an open brace, forget about any
    // prevailing indent and make sure it looks like the start
    // of a function

    if (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	goto theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f() {\n if (1)"
    if (cur_curpos.lnum < curbuf->b_ml.ml_line_count
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == NULL
	    && vim_strchr(theline, '}') == NULL
	    && !cin_ends_in(theline, (char_u *)":", NULL)
	    && !cin_ends_in(theline, (char_u *)",", NULL)
	    && cin_isfuncdecl(NULL, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	goto theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    while (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	if ((trypos = ind_find_start_CORS(NULL)) != NULL)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    continue;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	if (curbuf->b_ind_cpp_baseclass != 0 && theline[0] != '{')
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	if (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    break;
	}

	// Skip preprocessor directives and blank lines.
	if (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    continue;

	if (cin_nocode(l))
	    continue;

	// If the previous line ends in ',', use one level of
	// indentation:
	// int foo,
	//     bar;
	// do this before checking for '}' in case of eg.
	// enum foobar
	// {
	//   ...
	// } foo,
	//   bar;
	n = 0;
	if (cin_ends_in(l, (char_u *)",", NULL)
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    if (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != NULL)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    while (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		if (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    break;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    if (amount == 0)
		amount = cin_first_id_amount();
	    if (amount == 0)
		amount = ind_continuation;
	    break;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	if (cin_isfuncdecl(NULL, cur_curpos.lnum, 0))  // XXX
	    break;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	if (*skipwhite(l) == '}')
	    break;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	if (cin_ends_in(l, (char_u *)"};", NULL))
	    break;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	if (cin_ends_in(l, (char_u *)"[", NULL))
	{
	    amount = get_indent() + ind_continuation;
	    break;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	if (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    while (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		if (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    break;
	    }
	    if (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}", NULL))
		break;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	if (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    break;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	if (cin_ends_in(l, (char_u *)";", NULL))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    if (cin_ends_in(l, (char_u *)",", NULL)
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		break;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	if ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	break;
    }

    // add extra indent for a comment
    if (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    if (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	if (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    if (cur_amount > 0)
		amount = cur_amount;
	    else if (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    if (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    return amount;
}