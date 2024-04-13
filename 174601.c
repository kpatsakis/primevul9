compile_endfor(char_u *arg, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope = cctx->ctx_scope;
    forscope_T	*forscope;
    isn_T	*isn;

    if (misplaced_cmdmod(cctx))
	return NULL;

    if (scope == NULL || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	return NULL;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    if (cctx->ctx_skip != SKIP_YES)
    {
	unwind_locals(cctx, scope->se_local_count);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_top_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_top_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	if (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
	    return NULL;
    }

    vim_free(scope);

    return arg;
}