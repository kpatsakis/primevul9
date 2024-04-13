check_for_bool_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_BOOL
	    && !(args[idx].v_type == VAR_NUMBER
		&& (args[idx].vval.v_number == 0
		    || args[idx].vval.v_number == 1)))
    {
	semsg(_(e_bool_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}