reg_getline(linenr_T lnum)
{
    // when looking behind for a match/no-match lnum is negative.  But we
    // can't go before line 1
    if (rex.reg_firstlnum + lnum < 1)
	return NULL;
    if (lnum > rex.reg_maxline)
	// Must have matched the "\n" in the last line.
	return (char_u *)"";
    return ml_get_buf(rex.reg_buf, rex.reg_firstlnum + lnum, FALSE);
}