compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    if (misplaced_cmdmod(cctx))
	return NULL;

    // end block scope from :catch or :finally
    if (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    if (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	if (scope == NULL)
	    emsg(_(e_endtry_without_try));
	else if (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	else if (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	else
	    emsg(_(e_missing_endif));
	return NULL;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    if (cctx->ctx_skip != SKIP_YES)
    {
	if (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    return NULL;
	}

#ifdef FEAT_PROFILE
	if (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						.isn_type == ISN_PROF_START)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#endif

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							  instr->ga_len, cctx);

	if (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_data)
					   + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
    }

    // If there is a finally clause that ends in return then we will return.
    // If one of the blocks didn't end in "return" or we did not catch all
    // exceptions reset the had_return flag.
    if (!(scope->se_u.se_try.ts_has_finally && cctx->ctx_had_return)
	    && (scope->se_u.se_try.ts_no_return
		|| !scope->se_u.se_try.ts_caught_all))
	cctx->ctx_had_return = FALSE;

    compile_endblock(cctx);

    if (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	if (generate_instr(cctx, ISN_ENDTRY) == NULL)
	    return NULL;
#ifdef FEAT_PROFILE
	if (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    return arg;
}