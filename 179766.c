get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    if (p == NULL)
	return 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, NULL, NULL);
    return (int)col;
}