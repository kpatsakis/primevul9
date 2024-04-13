check_for_string_or_dict_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_STRING && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_or_dict_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}