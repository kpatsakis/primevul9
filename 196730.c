compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    int		ppconst_used = ppconst->pp_used;

    // eval the first expression
    if (compile_expr3(arg, cctx, ppconst) == FAIL)
	return FAIL;

    // || and && work almost the same
    return compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
}