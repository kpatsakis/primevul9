misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    if (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	return TRUE;
    }
    return FALSE;
}