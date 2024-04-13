current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    int		idx = instr->ga_len;

    while (idx > 0)
    {
	if (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    continue;
	}
#ifdef FEAT_PROFILE
	if (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    continue;
	}
#endif
	if (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    continue;
	}
	break;
    }
    return idx;
}