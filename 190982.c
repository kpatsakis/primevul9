var2fpos(
    typval_T	*varp,
    int		dollar_lnum,	// TRUE when $ is last line
    int		*fnum,		// set to fnum for '0, 'A, etc.
    int		charcol)	// return character column
{
    char_u		*name;
    static pos_T	pos;
    pos_T		*pp;

    // Argument can be [lnum, col, coladd].
    if (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	int		len;
	int		error = FALSE;
	listitem_T	*li;

	l = varp->vval.v_list;
	if (l == NULL)
	    return NULL;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	if (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_line_count)
	    return NULL;	// invalid line number
	if (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	else
	    len = (long)STRLEN(ml_get(pos.lnum));

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	if (li != NULL && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != NULL
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	else
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    if (error)
		return NULL;
	}

	// Accept a position up to the NUL after the line.
	if (pos.col == 0 || (int)pos.col > len + 1)
	    return NULL;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	if (error)
	    pos.coladd = 0;

	return &pos;
    }

    if (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	return NULL;

    name = tv_get_string_chk(varp);
    if (name == NULL)
	return NULL;
    if (name[0] == '.')				// cursor
    {
	pos = curwin->w_cursor;
	if (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	return &pos;
    }
    if (name[0] == 'v' && name[1] == NUL)	// Visual start
    {
	if (VIsual_active)
	    pos = VIsual;
	else
	    pos = curwin->w_cursor;
	if (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	return &pos;
    }
    if (name[0] == '\'')			// mark
    {
	pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	if (pp == NULL || pp == (pos_T *)-1 || pp->lnum <= 0)
	    return NULL;
	if (charcol)
	    pp->col = buf_byteidx_to_charidx(curbuf, pp->lnum, pp->col);
	return pp;
    }

    pos.coladd = 0;

    if (name[0] == 'w' && dollar_lnum)
    {
	pos.col = 0;
	if (name[1] == '0')		// "w0": first visible line
	{
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    return &pos;
	}
	else if (name[1] == '$')	// "w$": last visible line
	{
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    return &pos;
	}
    }
    else if (name[0] == '$')		// last column or line
    {
	if (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_line_count;
	    pos.col = 0;
	}
	else
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    if (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    else
		pos.col = (colnr_T)STRLEN(ml_get_curline());
	}
	return &pos;
    }
    if (in_vim9script())
	semsg(_(e_invalid_value_for_line_number_str), name);
    return NULL;
}