ref_param_write(iparam_list * plist, gs_param_name pkey, const ref * pvalue)
{
    ref kref;
    int code;

    if (!ref_param_requested((gs_param_list *) plist, pkey))
        return 0;
    code = ref_param_key(plist, pkey, &kref);
    if (code < 0)
        return code;
    return (*plist->u.w.write) (plist, &kref, pvalue);
}