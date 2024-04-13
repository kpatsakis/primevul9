array_param_enumerate(iparam_list * plist, gs_param_enumerator_t * penum,
                      gs_param_key_t * key, ref_type * type)
{
    int index = penum->intval;
    ref *bot = ((array_param_list *) plist)->bot;
    ref *ptr = bot + index;
    ref *top = ((array_param_list *) plist)->top;

    for (; ptr < top; ptr += 2) {
        index += 2;

        if (r_has_type(ptr, t_name)) {
            int code = ref_to_key(ptr, key, plist);

            *type = r_type(ptr);
            penum->intval = index;
            return code;
        }
    }
    return 1;
}