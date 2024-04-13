readdirex_dict_arg(typval_T *tv, int *cmp)
{
    char_u     *compare;

    if (tv->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	return FAIL;
    }

    if (dict_find(tv->vval.v_dict, (char_u *)"sort", -1) != NULL)
	compare = dict_get_string(tv->vval.v_dict, (char_u *)"sort", FALSE);
    else
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	return FAIL;
    }

    if (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    else if (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    else if (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    else if (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    return OK;
}