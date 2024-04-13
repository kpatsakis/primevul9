ref_to_key(const ref * pref, gs_param_key_t * key, iparam_list *plist)
{
    if (r_has_type(pref, t_name)) {
        ref nref;

        name_string_ref(plist->memory, pref, &nref);
        key->data = nref.value.const_bytes;
        key->size = r_size(&nref);
        key->persistent = false; /* names may be freed */
    } else if (r_has_type(pref, t_integer)) {
        char istr[sizeof(long) * 8 / 3 + 2];
        int len;
        byte *buf;

        gs_sprintf(istr, "%"PRIpsint, pref->value.intval);
        len = strlen(istr);
        /* GC will take care of freeing this: */
        buf = gs_alloc_string(plist->memory, len, "ref_to_key");
        if (!buf)
            return_error(gs_error_VMerror);
        key->data = buf;
        key->size = len;
        key->persistent = true;
    } else
        return_error(gs_error_typecheck);
    return 0;
}