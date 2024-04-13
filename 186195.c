ref_param_write_typed_array(gs_param_list * plist, gs_param_name pkey,
                            void *pvalue, uint count,
                            int (*make)(ref *, const void *, uint,
                                        gs_ref_memory_t *))
{
    iparam_list *const iplist = (iparam_list *) plist;
    ref value;
    uint i;
    ref *pe;
    int code;

    if ((code = ref_array_param_requested(iplist, pkey, &value, count,
                                       "ref_param_write_typed_array")) <= 0)
        return code;
    for (i = 0, pe = value.value.refs; i < count; ++i, ++pe)
        if ((code = (*make) (pe, pvalue, i, iplist->ref_memory)) < 0)
            return code;
    return ref_param_write(iplist, pkey, &value);
}