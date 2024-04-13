compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	int    error)	    // when TRUE may give error
{
    scriptitem_T    *si;
    int		    idx;
    imported_T	    *import;

    if (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	return FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx);
    if (idx == -1 || si->sn_version != SCRIPT_VERSION_VIM9)
    {
	// variable is not in sn_var_vals: old style script.
	return generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
    }
    if (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	return OK;
    }

    import = end == NULL ? NULL : find_imported(name, 0, cctx);
    if (import != NULL)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	int	cc;
	ufunc_T	*ufunc;
	type_T	*type;

	// Need to lookup the member.
	if (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    return FAIL;
	}
	++p;
	if (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    return FAIL;
	}

	// isolate one name
	exp_name = p;
	while (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	idx = find_exported(import->imp_sid, exp_name, &ufunc, &type,
								   cctx, TRUE);
	*p = cc;
	p = skipwhite(p);
	*end = p;

	if (idx < 0)
	{
	    if (ufunc != NULL)
	    {
		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, NULL);
		return OK;
	    }
	    return FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		import->imp_sid,
		idx,
		type);
	return OK;
    }

    if (error)
	semsg(_(e_item_not_found_str), name);
    return FAIL;
}