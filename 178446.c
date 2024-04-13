read_limits(long *minval, long *maxval)
{
    int		reverse = FALSE;
    char_u	*first_char;
    long	tmp;

    if (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	reverse = TRUE;
    }
    first_char = regparse;
    *minval = getdigits(&regparse);
    if (*regparse == ',')	    // There is a comma
    {
	if (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	else
	    *maxval = MAX_LIMIT;
    }
    else if (VIM_ISDIGIT(*first_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    else
	*maxval = MAX_LIMIT;	    // It was \{} or \{-}
    if (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    if (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_in_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    if ((!reverse && *minval > *maxval) || (reverse && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    return OK;
}