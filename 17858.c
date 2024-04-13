check_for_opt_buffer_or_dict_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_UNKNOWN
	    && args[idx].v_type != VAR_STRING
	    && args[idx].v_type != VAR_NUMBER
	    && args[idx].v_type != VAR_DICT)
    {
	semsg(_(e_string_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}