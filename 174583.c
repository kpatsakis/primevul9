compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    if (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	return NULL;
    }
    unwind_locals(cctx, scope->se_local_count);
    if (!cctx->ctx_had_return)
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    if (cctx->ctx_compile_type == CT_PROFILE)
    {
	if (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	if (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
	{
	    // the previous block was not executed, this one will, do count the
	    // "else" for profiling
	    cctx->ctx_skip = SKIP_NOT;
	    generate_instr(cctx, ISN_PROF_END);
	    generate_instr(cctx, ISN_PROF_START);
	    cctx->ctx_skip = SKIP_YES;
	}
    }
#endif

    if (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	if (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    if (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						    JUMP_ALWAYS, cctx) == FAIL)
		return NULL;
	}

	if (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    if (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	if (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    return p;
}