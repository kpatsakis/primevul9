ref_param_read(iparam_list * plist, gs_param_name pkey, iparam_loc * ploc,
               int type)
{
    iparam_list *const iplist = (iparam_list *) plist;
    ref kref;
    int code = ref_param_key(plist, pkey, &kref);

    if (code < 0)
        return code;
    code = (*plist->u.r.read) (iplist, &kref, ploc);
    if (code != 0)
        return code;
    if (type >= 0)
        iparam_check_type(*ploc, type);
    return 0;
}