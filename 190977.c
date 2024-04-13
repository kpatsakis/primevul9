typval2string(typval_T *tv, int convert)
{
    garray_T	ga;
    char_u	*retval;
#ifdef FEAT_FLOAT
    char_u	numbuf[NUMBUFLEN];
#endif

    if (convert && tv->v_type == VAR_LIST)
    {
	ga_init2(&ga, (int)sizeof(char), 80);
	if (tv->vval.v_list != NULL)
	{
	    list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE, 0);
	    if (tv->vval.v_list->lv_len > 0)
		ga_append(&ga, NL);
	}
	ga_append(&ga, NUL);
	retval = (char_u *)ga.ga_data;
    }
#ifdef FEAT_FLOAT
    else if (convert && tv->v_type == VAR_FLOAT)
    {
	vim_snprintf((char *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	retval = vim_strsave(numbuf);
    }
#endif
    else
	retval = vim_strsave(tv_get_string(tv));
    return retval;
}