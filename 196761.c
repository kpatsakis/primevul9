compile_lambda(char_u **arg, cctx_T *cctx)
{
    int		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg);
    if (r != OK)
    {
	clear_evalarg(&evalarg, NULL);
	return r;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    if (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    if (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // The last entry in evalarg.eval_tofree_ga is a copy of the last line and
    // "*arg" may point into it.  Point into the original line to avoid a
    // dangling pointer.
    if (evalarg.eval_using_cmdline)
    {
	garray_T    *gap = &evalarg.eval_tofree_ga;
	size_t	    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];

	*arg = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[cctx->ctx_lnum]
									 + off;
    }

    clear_evalarg(&evalarg, NULL);

    if (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	return generate_FUNCREF(cctx, ufunc);
    }

    func_ptr_unref(ufunc);
    return FAIL;
}