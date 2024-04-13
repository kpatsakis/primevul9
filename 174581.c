compile_mult_expr(char_u *arg, int cmdidx, cctx_T *cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*expr_start;
    int		count = 0;
    int		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;

    for (;;)
    {
	if (ends_excmd2(prev, p))
	    break;
	expr_start = p;
	if (compile_expr0(&p, cctx) == FAIL)
	    return NULL;

	if (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_stack(cctx, 0);
	    if (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_value_str), expr_start);
		return NULL;
	    }
	}

	++count;
	prev = p;
	p = skipwhite(p);
    }

    if (count > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	if (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    generate_ECHO(cctx, cmdidx == CMD_echo, count);
	else if (cmdidx == CMD_execute)
	    generate_MULT_EXPR(cctx, ISN_EXECUTE, count);
	else if (cmdidx == CMD_echomsg)
	    generate_MULT_EXPR(cctx, ISN_ECHOMSG, count);
	else if (cmdidx == CMD_echoconsole)
	    generate_MULT_EXPR(cctx, ISN_ECHOCONSOLE, count);
	else
	    generate_MULT_EXPR(cctx, ISN_ECHOERR, count);

	cctx->ctx_lnum = save_lnum;
    }
    return p;
}