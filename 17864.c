check_for_job_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_JOB)
    {
	semsg(_(e_job_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}