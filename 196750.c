compile_get_register(char_u **arg, cctx_T *cctx)
{
    int		ret;

    ++*arg;
    if (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	return FAIL;
    }
    if (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	return FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, NULL, &t_string);
    ++*arg;
    return ret;
}