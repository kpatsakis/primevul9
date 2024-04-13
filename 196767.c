compile_leader(cctx_T *cctx, int numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    while (p > start)
    {
	--p;
	while (VIM_ISWHITE(*p))
	    --p;
	if (*p == '-' || *p == '+')
	{
	    int		negate = *p == '-';
	    isn_T	*isn;
	    type_T	*type;

	    type = get_type_on_stack(cctx, 0);
	    if (type != &t_float && need_type(type, &t_number,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		return FAIL;

	    while (p > start && (p[-1] == '-' || p[-1] == '+'))
	    {
		--p;
		if (*p == '-')
		    negate = !negate;
	    }
	    // only '-' has an effect, for '+' we only check the type
	    if (negate)
	    {
		isn = generate_instr(cctx, ISN_NEGATENR);
		if (isn == NULL)
		    return FAIL;
	    }
	}
	else if (numeric_only)
	{
	    ++p;
	    break;
	}
	else
	{
	    int  invert = *p == '!';

	    while (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		if (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    if (generate_2BOOL(cctx, invert, -1) == FAIL)
		return FAIL;
	}
    }
    *end = p;
    return OK;
}