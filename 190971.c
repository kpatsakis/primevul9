eval_foldexpr(char_u *arg, int *cp)
{
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    int		use_sandbox = was_set_insecurely((char_u *)"foldexpr",
								   OPT_LOCAL);

    ++emsg_off;
    if (use_sandbox)
	++sandbox;
    ++textwinlock;
    *cp = NUL;
    if (eval0(arg, &tv, NULL, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    else
    {
	// If the result is a number, just return the number.
	if (tv.v_type == VAR_NUMBER)
	    retval = tv.vval.v_number;
	else if (tv.v_type != VAR_STRING || tv.vval.v_string == NULL)
	    retval = 0;
	else
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    if (!VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((char *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    if (use_sandbox)
	--sandbox;
    --textwinlock;
    clear_evalarg(&EVALARG_EVALUATE, NULL);

    return (int)retval;
}