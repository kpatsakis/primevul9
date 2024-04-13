dict_param_write(iparam_list * plist, const ref * pkey, const ref * pvalue)
{
    int code =
        dict_put(&((dict_param_list *) plist)->dict, pkey, pvalue, NULL);

    return min(code, 0);
}