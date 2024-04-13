find_last_paren(char_u *l, int start, int end)
{
    int		i;
    int		retval = FALSE;
    int		open_count = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    for (i = 0; l[i] != NUL; i++)
    {
	i = (int)(cin_skipcomment(l + i) - l); // ignore parens in comments
	i = (int)(skip_string(l + i) - l);    // ignore parens in quotes
	if (l[i] == start)
	    ++open_count;
	else if (l[i] == end)
	{
	    if (open_count > 0)
		--open_count;
	    else
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    return retval;
}