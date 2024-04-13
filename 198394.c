copy_func(char_u *lambda, char_u *global, ectx_T *ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, TRUE, NULL);
    ufunc_T *fp = NULL;

    if (ufunc == NULL)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	return FAIL;
    }

    fp = find_func(global, TRUE, NULL);
    if (fp != NULL)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_excl_to_replace), global);
	return FAIL;
    }

    fp = alloc_clear(offsetof(ufunc_T, uf_name) + STRLEN(global) + 1);
    if (fp == NULL)
	return FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    if (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_lines, &fp->uf_lines) == FAIL)
	goto failed;

    fp->uf_name_exp = ufunc->uf_name_exp == NULL ? NULL
					 : vim_strsave(ufunc->uf_name_exp);
    if (ufunc->uf_arg_types != NULL)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	if (fp->uf_arg_types == NULL)
	    goto failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    sizeof(type_T *) * fp->uf_args.ga_len);
    }
    if (ufunc->uf_va_name != NULL)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	if (fp->uf_va_name == NULL)
	    goto failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;
    STRCPY(fp->uf_name, global);
    hash_add(&func_hashtab, UF2HIKEY(fp));

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    if ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == NULL)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	if (pt == NULL)
	    goto failed;
	if (fill_partial_and_closure(pt, ufunc, ectx) == FAIL)
	{
            vim_free(pt);
	    goto failed;
	}
	ufunc->uf_partial = pt;
	--pt->pt_refcount;  // not actually referenced here
    }

    return OK;

failed:
    func_clear_free(fp, TRUE);
    return FAIL;
}