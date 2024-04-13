eval_number(
	char_u	    **arg,
	typval_T    *rettv,
	int	    evaluate,
	int	    want_string UNUSED)
{
    int		len;
    int		skip_quotes = !in_old_script(4);
#ifdef FEAT_FLOAT
    char_u	*p;
    int		get_float = FALSE;

    // We accept a float when the format matches
    // "[0-9]\+\.[0-9]\+\([eE][+-]\?[0-9]\+\)\?".  This is very
    // strict to avoid backwards compatibility problems.
    // With script version 2 and later the leading digit can be
    // omitted.
    // Don't look for a float after the "." operator, so that
    // ":let vers = 1.2.3" doesn't fail.
    if (**arg == '.')
	p = *arg;
    else
    {
	p = *arg + 1;
	if (skip_quotes)
	    for (;;)
	    {
		if (*p == '\'')
		    ++p;
		if (!vim_isdigit(*p))
		    break;
		p = skipdigits(p);
	    }
	else
	    p = skipdigits(p);
    }
    if (!want_string && p[0] == '.' && vim_isdigit(p[1]))
    {
	get_float = TRUE;
	p = skipdigits(p + 2);
	if (*p == 'e' || *p == 'E')
	{
	    ++p;
	    if (*p == '-' || *p == '+')
		++p;
	    if (!vim_isdigit(*p))
		get_float = FALSE;
	    else
		p = skipdigits(p + 1);
	}
	if (ASCII_ISALPHA(*p) || *p == '.')
	    get_float = FALSE;
    }
    if (get_float)
    {
	float_T	f;

	*arg += string2float(*arg, &f, skip_quotes);
	if (evaluate)
	{
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = f;
	}
    }
    else
#endif
    if (**arg == '0' && ((*arg)[1] == 'z' || (*arg)[1] == 'Z'))
    {
	char_u  *bp;
	blob_T  *blob = NULL;  // init for gcc

	// Blob constant: 0z0123456789abcdef
	if (evaluate)
	    blob = blob_alloc();
	for (bp = *arg + 2; vim_isxdigit(bp[0]); bp += 2)
	{
	    if (!vim_isxdigit(bp[1]))
	    {
		if (blob != NULL)
		{
		    emsg(_(e_blob_literal_should_have_an_even_number_of_hex_characters));
		    ga_clear(&blob->bv_ga);
		    VIM_CLEAR(blob);
		}
		return FAIL;
	    }
	    if (blob != NULL)
		ga_append(&blob->bv_ga,
			     (hex2nr(*bp) << 4) + hex2nr(*(bp+1)));
	    if (bp[2] == '.' && vim_isxdigit(bp[3]))
		++bp;
	}
	if (blob != NULL)
	    rettv_blob_set(rettv, blob);
	*arg = bp;
    }
    else
    {
	varnumber_T	n;

	// decimal, hex or octal number
	vim_str2nr(*arg, NULL, &len, skip_quotes
		      ? STR2NR_NO_OCT + STR2NR_QUOTE
		      : STR2NR_ALL, &n, NULL, 0, TRUE);
	if (len == 0)
	{
	    if (evaluate)
		semsg(_(e_invalid_expression_str), *arg);
	    return FAIL;
	}
	*arg += len;
	if (evaluate)
	{
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = n;
	}
    }
    return OK;
}