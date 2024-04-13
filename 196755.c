get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	int	    types_optional,
	evalarg_T   *evalarg)
{
    int		r;
    ufunc_T	*ufunc;
    int		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg);
    current_sctx.sc_version = save_sc_version;
    if (r != OK)
	return r;

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    if (ufunc->uf_ret_type == NULL || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, NULL);

    if (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	return OK;
    }
    clear_tv(rettv);
    return FAIL;
}