compile_for(char_u *arg_start, cctx_T *cctx)
{
    char_u	*arg;
    char_u	*arg_end;
    char_u	*name = NULL;
    char_u	*p;
    char_u	*wp;
    int		var_count = 0;
    int		var_list = FALSE;
    int		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    lvar_T	*loop_lvar;	// loop iteration variable
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    int		idx;
    int		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_count, &semicolon, FALSE);
    if (p == NULL)
	return NULL;
    if (var_count == 0)
	var_count = 1;
    else
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    if (may_get_next_line_error(wp, &p, cctx) == FAIL)
	return NULL;
    if (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	if (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	else
	    emsg(_(e_missing_in_after_for));
	return NULL;
    }
    wp = p + 2;
    if (may_get_next_line_error(wp, &p, cctx) == FAIL)
	return NULL;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    if (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    if (scope == NULL)
	return NULL;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, FALSE, &t_number);
    if (loop_lvar == NULL)
    {
	// out of memory
	drop_scope(cctx);
	return NULL;
    }
    generate_STORENR(cctx, loop_lvar->lv_idx, -1);

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    if (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	return NULL;
    }
    arg_end = arg;

    if (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_stack(cctx, 0);
	if (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    return NULL;
	}

	if (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	else if (vartype->tt_type == VAR_BLOB)
	    item_type = &t_number;
	else if (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    if (!var_list)
		item_type = vartype->tt_member;
	    else if (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	scope->se_u.se_for.fs_top_label = current_instr_idx(cctx);

	if (cctx->ctx_compile_type == CT_DEBUG)
	{
	    int		save_prev_lnum = cctx->ctx_prev_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_prev_lnum = prev_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_names_len;
	    cctx->ctx_prev_lnum = save_prev_lnum;
	}

	generate_FOR(cctx, loop_lvar->lv_idx);

	arg = arg_start;
	if (var_list)
	{
	    generate_UNPACK(cctx, var_count, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_stack.ga_len;

	    // add type of the items
	    for (idx = 0; idx < var_count; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		if (push_type_stack(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    return NULL;
		}
	    }
	}

	for (idx = 0; idx < var_count; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    int			opt_flags = 0;
	    int			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    if (name == NULL)
		goto failed;
	    if (*p == ':')
	    {
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, TRUE);
	    }

	    if (get_var_dest(name, &dest, CMD_for, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		goto failed;
	    if (dest != dest_local)
	    {
		if (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     0, 0, type, name) == FAIL)
		    goto failed;
	    }
	    else if (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		if (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
		    goto failed;
	    }
	    else
	    {
		// Script var is not supported.
		if (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    goto failed;
		}

		if (!valid_varname(arg, (int)varlen, FALSE))
		    goto failed;
		if (lookup_local(arg, varlen, NULL, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared), arg);
		    goto failed;
		}

		// Reserve a variable to store "var".
		where.wt_index = var_list ? idx + 1 : 0;
		where.wt_variable = TRUE;
		if (lhs_type == &t_any)
		    lhs_type = item_type;
		else if (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    goto failed;
		var_lvar = reserve_local(cctx, arg, varlen, TRUE, lhs_type);
		if (var_lvar == NULL)
		    // out of memory or used as an argument
		    goto failed;

		if (semicolon && idx == var_count - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, NULL);
	    }

	    if (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}
    }

    return arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    return NULL;
}