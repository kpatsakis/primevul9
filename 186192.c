ref_param_key(const iparam_list * plist, gs_param_name pkey, ref * pkref)
{
    if (plist->int_keys) {
        long key;

        if (sscanf(pkey, "%ld", &key) != 1)
            return_error(gs_error_rangecheck);
        make_int(pkref, key);
        return 0;
    } else
        return name_ref(plist->memory, (const byte *)pkey, strlen(pkey), pkref, 0);
}