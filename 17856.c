check_for_string_or_func_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_PARTIAL
	    && args[idx].v_type != VAR_FUNC
	    && args[idx].v_type != VAR_STRING)
    {
	semsg(_(e_string_or_function_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}