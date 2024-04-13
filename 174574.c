compile_throw(char_u *arg, cctx_T *cctx UNUSED)
{
    char_u *p = skipwhite(arg);

    if (compile_expr0(&p, cctx) == FAIL)
	return NULL;
    if (cctx->ctx_skip == SKIP_YES)
	return p;
    if (may_generate_2STRING(-1, FALSE, cctx) == FAIL)
	return NULL;
    if (generate_instr_drop(cctx, ISN_THROW, 1) == NULL)
	return NULL;

    return p;
}