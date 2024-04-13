ref_param_begin_read_collection(gs_param_list * plist, gs_param_name pkey,
                                gs_param_dict * pvalue,
                                gs_param_collection_type_t coll_type)
{
    iparam_list *const iplist = (iparam_list *) plist;
    iparam_loc loc;
    bool int_keys = coll_type != 0;
    int code = ref_param_read(iplist, pkey, &loc, -1);
    dict_param_list *dlist;

    if (code != 0)
        return code;
    dlist = (dict_param_list *)
        gs_alloc_bytes(plist->memory, size_of(dict_param_list),
                       "ref_param_begin_read_collection");
    if (dlist == 0)
        return_error(gs_error_VMerror);
    if (r_has_type(loc.pvalue, t_dictionary)) {
        code = dict_param_list_read(dlist, loc.pvalue, NULL, false,
                                    iplist->ref_memory);
        dlist->int_keys = int_keys;
        if (code >= 0)
            pvalue->size = dict_length(loc.pvalue);
    } else if (int_keys && r_is_array(loc.pvalue)) {
        code = array_indexed_param_list_read(dlist, loc.pvalue, NULL, false,
                                             iplist->ref_memory);
        if (code >= 0)
            pvalue->size = r_size(loc.pvalue);
    } else
        code = gs_note_error(gs_error_typecheck);
    if (code < 0) {
        gs_free_object(plist->memory, dlist, "ref_param_begin_write_collection");
        return iparam_note_error(loc, code);
    }
    pvalue->list = (gs_param_list *) dlist;
    return 0;
}