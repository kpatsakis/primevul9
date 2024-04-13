compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    int		count = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    int		is_const;
    int		is_all_const = TRUE;	// reset when non-const encountered

    if (d == NULL)
	return FAIL;
    if (generate_ppconst(cctx, ppconst) == FAIL)
	return FAIL;
    for (;;)
    {
	char_u	    *key = NULL;

	if (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}

	if (**arg == '}')
	    break;

	if (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    if (compile_expr0(arg, cctx) == FAIL)
		return FAIL;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    if (isn->isn_type == ISN_PUSHNR)
	    {
		char buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.number);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    if (isn->isn_type == ISN_PUSHS)
		key = isn->isn_arg.string;
	    else if (may_generate_2STRING(-1, FALSE, cctx) == FAIL)
		return FAIL;
	    *arg = skipwhite(*arg);
	    if (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_key));
		return FAIL;
	    }
	    ++*arg;
	}
	else
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    key = get_literal_key(arg);
	    if (key == NULL)
		return FAIL;
	    if (generate_PUSHS(cctx, &key) == FAIL)
		return FAIL;
	}

	// Check for duplicate keys, if using string keys.
	if (key != NULL)
	{
	    item = dict_find(d, key, -1);
	    if (item != NULL)
	    {
		semsg(_(e_duplicate_key_in_dicitonary), key);
		goto failret;
	    }
	    item = dictitem_alloc(key);
	    if (item != NULL)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		if (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	if (**arg != ':')
	{
	    if (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    else
		semsg(_(e_missing_colon_in_dictionary), *arg);
	    return FAIL;
	}
	whitep = *arg + 1;
	if (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    return FAIL;
	}

	if (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}

	if (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    return FAIL;
	if (!is_const)
	    is_all_const = FALSE;
	++count;

	whitep = *arg;
	if (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}
	if (**arg == '}')
	    break;
	if (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary), *arg);
	    goto failret;
	}
	if (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    return FAIL;
	}
	whitep = *arg + 1;
	if (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    return FAIL;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    if (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    return generate_NEWDICT(cctx, count);

failret:
    if (*arg == NULL)
    {
	semsg(_(e_missing_dict_end), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    return FAIL;
}