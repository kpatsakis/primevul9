script_get(exarg_T *eap UNUSED, char_u *cmd UNUSED)
{
#ifdef FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    if (cmd[0] != '<' || cmd[1] != '<' || eap->getline == NULL)
	return NULL;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE);
    if (l == NULL)
	return NULL;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    return (char_u *)ga.ga_data;
#else
    return NULL;
#endif
}