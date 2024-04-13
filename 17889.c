check_for_opt_string_or_number_arg(typval_T *args, int idx)
{
    return (args[idx].v_type == VAR_UNKNOWN
	    || check_for_string_or_number_arg(args, idx) != FAIL);
}