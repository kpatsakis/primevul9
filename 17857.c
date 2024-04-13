check_for_dict_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_dict_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}