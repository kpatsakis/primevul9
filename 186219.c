ref_param_requested(const gs_param_list * plist, gs_param_name pkey)
{
    const iparam_list *const ciplist = (const iparam_list *)plist;
    ref kref;
    ref *ignore_value;

    if (!r_has_type(&ciplist->u.w.wanted, t_dictionary))
        return -1;
    if (ref_param_key(ciplist, pkey, &kref) < 0)
        return -1;		/* catch it later */
    return (dict_find(&ciplist->u.w.wanted, &kref, &ignore_value) > 0);
}