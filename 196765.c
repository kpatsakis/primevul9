compile_string(isn_T *isn, cctx_T *cctx)
{
    char_u	*s = isn->isn_arg.string;
    garray_T	save_ga = cctx->ctx_instr;
    int		expr_res;
    int		trailing_error;
    int		instr_count;
    isn_T	*instr = NULL;

    // Remove the string type from the stack.
    --cctx->ctx_type_stack.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = NULL;
    expr_res = compile_expr0(&s, cctx);
    s = skipwhite(s);
    trailing_error = *s != NUL;

    if (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	if (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_stack.ga_len;
	return FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_count = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_count].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    return OK;
}