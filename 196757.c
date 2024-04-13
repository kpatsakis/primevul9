compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = NULL;

    // Recognize <type>
    if (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, TRUE);
	if (want_type == NULL)
	    return FAIL;

	if (**arg != '>')
	{
	    if (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    else
		emsg(_(e_missing_gt));
	    return FAIL;
	}
	++*arg;
	if (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	    return FAIL;
    }

    if (compile_expr8(arg, cctx, ppconst) == FAIL)
	return FAIL;

    if (want_type != NULL)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	generate_ppconst(cctx, ppconst);
	actual = get_type_on_stack(cctx, 0);
	if (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    if (need_type(actual, want_type, -1, 0, cctx, FALSE, FALSE)
								       == FAIL)
		return FAIL;
	}
    }

    return OK;
}