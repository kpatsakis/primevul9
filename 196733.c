compile_expr0_ext(char_u **arg,  cctx_T *cctx, int *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    if (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	return FAIL;
    }
    if (is_const != NULL)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    if (generate_ppconst(cctx, &ppconst) == FAIL)
	return FAIL;
    return OK;
}