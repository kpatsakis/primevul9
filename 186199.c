stack_param_enumerate(iparam_list * plist, gs_param_enumerator_t * penum,
                      gs_param_key_t * key, ref_type * type)
{
    int code;
    stack_param_list *const splist = (stack_param_list *) plist;
    int index = penum->intval;
    ref *stack_element;

    do {
        stack_element =
            ref_stack_index(splist->pstack, index + 1 + splist->skip);
        if (!stack_element)
            return 1;
    } while (index += 2, !r_has_type(stack_element, t_name));
    *type = r_type(stack_element);
    code = ref_to_key(stack_element, key, plist);
    penum->intval = index;
    return code;
}