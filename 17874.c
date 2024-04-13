check_for_unknown_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments), idx + 1);
	return FAIL;
    }
    return OK;
}