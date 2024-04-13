compile_cexpr(char_u *line, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    if (isn == NULL)
	return NULL;
    isn->isn_arg.number = eap->cmdidx;

    p = eap->arg;
    if (compile_expr0(&p, cctx) == FAIL)
	return NULL;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    if (isn == NULL)
	return NULL;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    if (isn->isn_arg.cexpr.cexpr_ref == NULL)
	return NULL;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(line));

    return p;
}