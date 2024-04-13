ref_param_read_array(iparam_list * plist, gs_param_name pkey, iparam_loc * ploc)
{
    int code = ref_param_read(plist, pkey, ploc, -1);

    if (code != 0)
        return code;
    if (!r_is_array(ploc->pvalue))
        return iparam_note_error(*ploc, gs_error_typecheck);
    iparam_check_read(*ploc);
    return 0;
}