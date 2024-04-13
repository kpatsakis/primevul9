check_for_opt_buffer_arg(typval_T *args, int idx)
{
    return (args[idx].v_type == VAR_UNKNOWN
	    || check_for_buffer_arg(args, idx));
}