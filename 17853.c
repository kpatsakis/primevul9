check_for_float_or_nr_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_FLOAT && args[idx].v_type != VAR_NUMBER)
    {
	semsg(_(e_float_or_number_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}