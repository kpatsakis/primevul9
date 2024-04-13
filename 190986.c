eval_index_inner(
	typval_T    *rettv,
	int	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	int	    exclusive,
	char_u	    *key,
	int	    keylen,
	int	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    if (var1 != NULL && rettv->v_type != VAR_DICT)
	n1 = tv_get_number(var1);

    if (is_range)
    {
	if (rettv->v_type == VAR_DICT)
	{
	    if (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    return FAIL;
	}
	if (var2 != NULL)
	    n2 = tv_get_number(var2);
	else
	    n2 = VARNUM_MAX;
    }

    switch (rettv->v_type)
    {
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_FUNC:
	case VAR_PARTIAL:
	case VAR_FLOAT:
	case VAR_BOOL:
	case VAR_SPECIAL:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	    break; // not evaluating, skipping over subscript

	case VAR_NUMBER:
	case VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		if (in_vim9script() || exclusive)
		{
		    if (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    else
			s = char_from_string(s, n1);
		}
		else if (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    if (n1 < 0)
		    {
			n1 = len + n1;
			if (n1 < 0)
			    n1 = 0;
		    }
		    if (n2 < 0)
			n2 = len + n2;
		    else if (n2 >= len)
			n2 = len;
		    if (n1 >= len || n2 < 0 || n1 > n2)
			s = NULL;
		    else
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		else
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    if (n1 >= len || n1 < 0)
			s = NULL;
		    else
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    break;

	case VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    break;

	case VAR_LIST:
	    if (var1 == NULL)
		n1 = 0;
	    if (var2 == NULL)
		n2 = VARNUM_MAX;
	    if (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		return FAIL;
	    break;

	case VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		if (key == NULL)
		{
		    key = tv_get_string_chk(var1);
		    if (key == NULL)
			return FAIL;
		}

		item = dict_find(rettv->vval.v_dict, key, keylen);

		if (item == NULL)
		{
		    if (verbose)
		    {
			if (keylen > 0)
			    key[keylen] = NUL;
			semsg(_(e_dictkey), key);
		    }
		    return FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    break;
    }
    return OK;
}