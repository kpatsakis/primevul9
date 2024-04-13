ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*fname;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr;
    int		instr_count;
    int		is_global = FALSE;
    compiletype_T compile_type = CT_NONE;

    if (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    else if (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    if (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(void)getdigits(&arg);
	fname = vim_strnsave(eap->arg, arg - eap->arg);
    }
    else
	fname = trans_function_name(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD, NULL, NULL, NULL);
    if (fname == NULL)
    {
	semsg(_(e_invalid_argument_str), eap->arg);
	return;
    }

    ufunc = find_func(fname, is_global, NULL);
    if (ufunc == NULL)
    {
	char_u *p = untrans_function_name(fname);

	if (p != NULL)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE, NULL);
    }
    vim_free(fname);
    if (ufunc == NULL)
    {
	semsg(_(e_cannot_find_function_str), eap->arg);
	return;
    }
    if (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, NULL) == FAIL)
	return;
    if (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	return;
    }
    msg((char *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    switch (compile_type)
    {
	case CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_count = dfunc->df_instr_prof_count;
	    break;
#endif
	    // FALLTHROUGH
	case CT_NONE:
	    instr = dfunc->df_instr;
	    instr_count = dfunc->df_instr_count;
	    break;
	case CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_count = dfunc->df_instr_debug_count;
	    break;
    }

    list_instructions("", instr, instr_count, ufunc);
}