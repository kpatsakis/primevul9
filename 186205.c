ref_param_end_read_collection(gs_param_list * plist, gs_param_name pkey,
                              gs_param_dict * pvalue)
{
    iparam_list_release((dict_param_list *) pvalue->list);
    gs_free_object(plist->memory, pvalue->list,
                   "ref_param_end_read_collection");
    return 0;
}