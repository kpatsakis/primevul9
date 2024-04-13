find_match_paren_after_brace (int ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    if (trypos != NULL)
    {
	pos_T	*tryposBrace = find_start_brace();

	// If both an unmatched '(' and '{' is found.  Ignore the '('
	// position if the '{' is further down.
	if (tryposBrace != NULL
		&& (trypos->lnum != tryposBrace->lnum
		    ? trypos->lnum < tryposBrace->lnum
		    : trypos->col < tryposBrace->col))
	    trypos = NULL;
    }
    return trypos;
}