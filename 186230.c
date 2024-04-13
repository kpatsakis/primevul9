ref_param_read_commit(gs_param_list * plist)
{
    iparam_list *const iplist = (iparam_list *) plist;
    int i;
    int ecode = 0;

    if (!iplist->u.r.require_all)
        return 0;
    /* Check to make sure that all parameters were actually read. */
    for (i = 0; i < iplist->count; ++i)
        if (iplist->results[i] == 0)
            iplist->results[i] = ecode = gs_note_error(gs_error_undefined);
    return ecode;
}