check_for_opt_job_arg(typval_T *args, int idx)
{
    return (args[idx].v_type == VAR_UNKNOWN
	    || check_for_job_arg(args, idx) != FAIL);
}