check_for_string_or_number_or_list_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_LIST)
    {
	semsg(_(e_string_number_or_list_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}