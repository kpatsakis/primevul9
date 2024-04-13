array_new_indexed_plist_write(dict_param_list * plist, ref * parray,
                              const ref * pwanted, gs_ref_memory_t *imem)
{
    check_array(*parray);
    check_write(*parray);
    plist->u.w.write = array_new_indexed_param_write;
    ref_param_write_init((iparam_list *) plist, pwanted, imem);
    plist->dict = *parray;
    plist->int_keys = true;
    return 0;
}