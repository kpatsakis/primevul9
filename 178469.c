match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    int	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    int		len;
    char_u	*p;

    if (bytelen != NULL)
	*bytelen = 0;
    for (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	if (rex.line != reg_tofree)
	{
	    len = (int)STRLEN(rex.line);
	    if (reg_tofree == NULL || len >= (int)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		if (reg_tofree == NULL)
		    return RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.line);
	    rex.input = reg_tofree + (rex.input - rex.line);
	    rex.line = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	if (clnum == end_lnum)
	    len = end_col - ccol;
	else
	    len = (int)STRLEN(p + ccol);

	if (cstrncmp(p + ccol, rex.input, &len) != 0)
	    return RA_NOMATCH;  // doesn't match
	if (bytelen != NULL)
	    *bytelen += len;
	if (clnum == end_lnum)
	    break;		// match and at end!
	if (rex.lnum >= rex.reg_maxline)
	    return RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	if (bytelen != NULL)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	if (got_int)
	    return RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    return RA_MATCH;
}