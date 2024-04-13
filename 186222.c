ref_param_read_string_value(gs_memory_t *mem, const iparam_loc * ploc, gs_param_string * pvalue)
{
    const ref *pref = ploc->pvalue;

    switch (r_type(pref)) {
        case t_name: {
            ref nref;

            name_string_ref(mem, pref, &nref);
            pvalue->data = nref.value.const_bytes;
            pvalue->size = r_size(&nref);
            pvalue->persistent = true;
        }
            break;
        case t_string:
            iparam_check_read(*ploc);
            pvalue->data = pref->value.const_bytes;
            pvalue->size = r_size(pref);
            pvalue->persistent = false;
            break;
        case t_astruct:
            /* Note: technically, instead of the "mem" argument, we
               should be using the plists's ref_memory. However, in a
               simple call to .putdeviceparams, they are identical. */
            iparam_check_read(*ploc);
            if (gs_object_type(mem, pref->value.pstruct) != &st_bytes)
                return iparam_note_error(*ploc, gs_error_typecheck);
            pvalue->data = r_ptr(pref, byte);
            pvalue->size = gs_object_size(mem, pref->value.pstruct);
            pvalue->persistent = false;
            break;
        default:
            return iparam_note_error(*ploc, gs_error_typecheck);
    }
    return 0;
}