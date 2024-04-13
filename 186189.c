ref_param_get_next_key(gs_param_list * plist, gs_param_enumerator_t * penum,
                       gs_param_key_t * key)
{
    ref_type keytype;		/* result not needed here */
    iparam_list *const pilist = (iparam_list *) plist;

    return (*pilist->enumerate) (pilist, penum, key, &keytype);
}