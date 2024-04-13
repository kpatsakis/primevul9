check_for_list_or_blob_arg(typval_T *args, int idx)
{
    if (args[idx].v_type != VAR_LIST && args[idx].v_type != VAR_BLOB)
    {
	semsg(_(e_list_or_blob_required_for_argument_nr), idx + 1);
	return FAIL;
    }
    return OK;
}