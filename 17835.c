check_for_string_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}