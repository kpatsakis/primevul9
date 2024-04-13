trans_function_name(
    char_u	**pp,
    int		*is_global,
    int		skip,		// only find the end, don't evaluate
    int		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type)		// return: type of funcref if not NULL
{
    char_u	*name = NULL;
    char_u	*start;
    char_u	*end;
    int		lead;
    char_u	sid_buf[20];
    int		len;
    int		extra = 0;
    lval_T	lv;
    int		vim9script;

    if (fdp != NULL)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    if ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (int)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	return vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    if (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, NULL, &lv, FALSE, skip, flags | GLV_READ_ONLY,
					      lead > 2 ? 0 : FNE_CHECK_START);
    if (end == start)
    {
	if (!skip)
	    emsg(_(e_function_name_required));
	goto theend;
    }
    if (end == NULL || (lv.ll_tv != NULL && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	if (!aborting())
	{
	    if (end != NULL)
		semsg(_(e_invarg2), start);
	}
	else
	    *pp = find_name_end(start, NULL, NULL, FNE_INCL_BR);
	goto theend;
    }

    if (lv.ll_tv != NULL)
    {
	if (fdp != NULL)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = NULL;
	    fdp->fd_di = lv.ll_di;
	}
	if (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != NULL)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	else if (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != NULL)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    if (partial != NULL)
		*partial = lv.ll_tv->vval.v_partial;
	}
	else
	{
	    if (!skip && !(flags & TFN_QUIET) && (fdp == NULL
			     || lv.ll_dict == NULL || fdp->fd_newkey == NULL))
		emsg(_(e_funcref));
	    else
		*pp = end;
	    name = NULL;
	}
	goto theend;
    }

    if (lv.ll_name == NULL)
    {
	// Error found, but continue after the function name.
	*pp = end;
	goto theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    if (lv.ll_exp_name != NULL)
    {
	len = (int)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
						flags & TFN_NO_AUTOLOAD, NULL);
	if (name == lv.ll_exp_name)
	    name = NULL;
    }
    else if (!(flags & TFN_NO_DEREF))
    {
	len = (int)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
						flags & TFN_NO_AUTOLOAD, NULL);
	if (name == *pp)
	    name = NULL;
    }
    if (name != NULL)
    {
	name = vim_strsave(name);
	*pp = end;
	if (STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (int)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	goto theend;
    }

    if (lv.ll_exp_name != NULL)
    {
	len = (int)STRLEN(lv.ll_exp_name);
	if (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    else
    {
	// skip over "s:" and "g:"
	if (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    if (is_global != NULL && lv.ll_name[0] == 'g')
		*is_global = TRUE;
	    lv.ll_name += 2;
	}
	len = (int)(end - lv.ll_name);
    }
    if (len <= 0)
    {
	if (!skip)
	    emsg(_(e_function_name_required));
	goto theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().
    vim9script = ASCII_ISUPPER(*start) && in_vim9script();
    if (vim9script)
    {
	char_u *p;

	// SomeScript#func() is a global function.
	for (p = start; *p != NUL && *p != '('; ++p)
	    if (*p == AUTOLOAD_CHAR)
		vim9script = FALSE;
    }

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    if (skip)
	lead = 0;	// do nothing
    else if (lead > 0 || vim9script)
    {
	if (!vim9script)
	    lead = 3;
	if (vim9script || (lv.ll_exp_name != NULL
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    // It's script-local, "s:" or "<SID>"
	    if (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		goto theend;
	    }
	    sprintf((char *)sid_buf, "%ld_", (long)current_sctx.sc_sid);
	    if (vim9script)
		extra = 3 + (int)STRLEN(sid_buf);
	    else
		lead += (int)STRLEN(sid_buf);
	}
    }
    else if (!(flags & TFN_INT) && (builtin_function(lv.ll_name, len)
				   || (in_vim9script() && *lv.ll_name == '_')))
    {
	semsg(_(e_function_name_must_start_with_capital_or_s_str), start);
	goto theend;
    }
    if (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	if (cp != NULL && cp < end)
	{
	    semsg(_("E884: Function name cannot contain a colon: %s"), start);
	    goto theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    if (name != NULL)
    {
	if (!skip && (lead > 0 || vim9script))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (int)KE_SNR;
	    if (vim9script || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = end;

theend:
    clear_lval(&lv);
    return name;
}