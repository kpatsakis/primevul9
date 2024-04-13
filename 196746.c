generate_funcref(cctx_T *cctx, char_u *name)
{
    ufunc_T *ufunc = find_func(name, FALSE, cctx);

    if (ufunc == NULL)
	return FAIL;

    // Need to compile any default values to get the argument types.
    if (func_needs_compiling(ufunc, COMPILE_TYPE(ufunc))
	    && compile_def_function(ufunc, TRUE, COMPILE_TYPE(ufunc), NULL)
								       == FAIL)
	return FAIL;
    return generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type);
}