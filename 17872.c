check_for_nonempty_string_arg(typval_T *args, int idx)
{
    if (check_for_string_arg(args, idx) == FAIL)
	return FAIL;
    if (args[idx].vval.v_string == NULL || *args[idx].vval.v_string == NUL)
    {
	semsg(_(e_non_empty_string_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}