skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    int		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    if (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	if (l == NULL)		// just in case
	    l = ml_get_curline();
    }
    else
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    return amount;
}