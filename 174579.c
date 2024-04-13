compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, NULL);
    char_u *p = skipdigits(eap->cmd);

    if (p == range_end)
	return FAIL;
    return generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}