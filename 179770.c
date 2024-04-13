get_baseclass_amount(int col)
{
    int		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    if (col == 0)
    {
	amount = get_indent();
	if (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	if (!cin_ends_in(ml_get_curline(), (char_u *)",", NULL))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    else
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, NULL, NULL);
	amount = (int)vcol;
    }
    if (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    return amount;
}