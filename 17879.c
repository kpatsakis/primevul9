check_for_opt_bool_arg(typval_T *args, int idx)
{
    if (args[idx].v_type == VAR_UNKNOWN)
	return OK;
    return check_for_bool_arg(args, idx);
}