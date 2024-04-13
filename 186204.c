ref_param_begin_write_collection(gs_param_list * plist, gs_param_name pkey,
                                 gs_param_dict * pvalue,
                                 gs_param_collection_type_t coll_type)
{
    iparam_list *const iplist = (iparam_list *) plist;
    gs_ref_memory_t *imem = iplist->ref_memory;
    dict_param_list *dlist = (dict_param_list *)
        gs_alloc_bytes(plist->memory, size_of(dict_param_list),
                       "ref_param_begin_write_collection");
    int code;

    if (dlist == 0)
        return_error(gs_error_VMerror);
    if (coll_type != gs_param_collection_array) {
        ref dref;

        code = dict_alloc(imem, pvalue->size, &dref);
        if (code >= 0) {
            code = dict_param_list_write(dlist, &dref, NULL, imem);
            dlist->int_keys = coll_type == gs_param_collection_dict_int_keys;
        }
    } else {
        ref aref;

        code = gs_alloc_ref_array(imem, &aref, a_all, pvalue->size,
                                  "ref_param_begin_write_collection");
        if (code >= 0)
            code = array_new_indexed_plist_write(dlist, &aref, NULL, imem);
    }
    if (code < 0)
        gs_free_object(plist->memory, dlist, "ref_param_begin_write_collection");
    else
        pvalue->list = (gs_param_list *) dlist;
    return code;
}