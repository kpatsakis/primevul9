ref_param_read_get_policy(gs_param_list * plist, gs_param_name pkey)
{
    iparam_list *const iplist = (iparam_list *) plist;
    ref *pvalue;

    if (!(r_has_type(&iplist->u.r.policies, t_dictionary) &&
          dict_find_string(&iplist->u.r.policies, pkey, &pvalue) > 0 &&
          r_has_type(pvalue, t_integer))
        )
        return gs_param_policy_ignore;
    return (int)pvalue->value.intval;
}