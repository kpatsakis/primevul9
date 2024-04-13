eval_lit_string(char_u **arg, typval_T *rettv, int evaluate, int interpolate)
{
    char_u	*p;
    char_u	*str;
    int		reduce = interpolate ? -1 : 0;
    int		off = interpolate ? 0 : 1;

    // Find the end of the string, skipping ''.
    for (p = *arg + off; *p != NUL; MB_PTR_ADV(p))
    {
	if (*p == '\'')
	{
	    if (p[1] != '\'')
		break;
	    ++reduce;
	    ++p;
	}
	else if (interpolate)
	{
	    if (*p == '{')
	    {
		if (p[1] != '{')
		    break;
		++p;
		++reduce;
	    }
	    else if (*p == '}')
	    {
		++p;
		if (*p != '}')
		{
		    semsg(_(e_stray_closing_curly_str), *arg);
		    return FAIL;
		}
		++reduce;
	    }
	}
    }

    if (*p != '\'' && !(interpolate && *p == '{'))
    {
	semsg(_(e_missing_single_quote_str), *arg);
	return FAIL;
    }

    // If only parsing return after setting "*arg"
    if (!evaluate)
    {
	*arg = p + off;
	return OK;
    }

    // Copy the string into allocated memory, handling '' to ' reduction and
    // any expressions.
    str = alloc((p - *arg) - reduce);
    if (str == NULL)
	return FAIL;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = str;

    for (p = *arg + off; *p != NUL; )
    {
	if (*p == '\'')
	{
	    if (p[1] != '\'')
		break;
	    ++p;
	}
	else if (interpolate && (*p == '{' || *p == '}'))
	{
	    if (*p == '{' && p[1] != '{')
		break;
	    ++p;
	}
	MB_COPY_CHAR(p, str);
    }
    *str = NUL;
    *arg = p + off;

    return OK;
}