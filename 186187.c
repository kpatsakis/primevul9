dict_param_list_write(dict_param_list *plist, ref *pdict, const ref *pwanted,
                      gs_ref_memory_t *imem)
{
    check_dict_write(*pdict);
    plist->u.w.write = dict_param_write;
    plist->enumerate = dict_param_enumerate;
    ref_param_write_init((iparam_list *) plist, pwanted, imem);
    plist->dict = *pdict;
    return 0;
}