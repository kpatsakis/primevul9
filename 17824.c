check_for_opt_lnum_arg(typval_T *args, int idx)
{
    return (args[idx].v_type == VAR_UNKNOWN
	    || check_for_lnum_arg(args, idx));
}