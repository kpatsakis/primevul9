tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    int		failed = FALSE;

    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    if (tv2->v_type != VAR_FUNC && tv2->v_type != VAR_DICT
		    && ((tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)
								|| *op == '.'))
    {
	switch (tv1->v_type)
	{
	    case VAR_UNKNOWN:
	    case VAR_ANY:
	    case VAR_VOID:
	    case VAR_DICT:
	    case VAR_FUNC:
	    case VAR_PARTIAL:
	    case VAR_BOOL:
	    case VAR_SPECIAL:
	    case VAR_JOB:
	    case VAR_CHANNEL:
	    case VAR_INSTR:
		break;

	    case VAR_BLOB:
		if (*op != '+' || tv2->v_type != VAR_BLOB)
		    break;
		// BLOB += BLOB
		if (tv1->vval.v_blob != NULL && tv2->vval.v_blob != NULL)
		{
		    blob_T  *b1 = tv1->vval.v_blob;
		    blob_T  *b2 = tv2->vval.v_blob;
		    int	i, len = blob_len(b2);
		    for (i = 0; i < len; i++)
			ga_append(&b1->bv_ga, blob_get(b2, i));
		}
		return OK;

	    case VAR_LIST:
		if (*op != '+' || tv2->v_type != VAR_LIST)
		    break;
		// List += List
		if (tv2->vval.v_list != NULL)
		{
		    if (tv1->vval.v_list == NULL)
		    {
			tv1->vval.v_list = tv2->vval.v_list;
			++tv1->vval.v_list->lv_refcount;
		    }
		    else
			list_extend(tv1->vval.v_list, tv2->vval.v_list, NULL);
		}
		return OK;

	    case VAR_NUMBER:
	    case VAR_STRING:
		if (tv2->v_type == VAR_LIST)
		    break;
		if (vim_strchr((char_u *)"+-*/%", *op) != NULL)
		{
		    // nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
		    n = tv_get_number(tv1);
#ifdef FEAT_FLOAT
		    if (tv2->v_type == VAR_FLOAT)
		    {
			float_T f = n;

			if (*op == '%')
			    break;
			switch (*op)
			{
			    case '+': f += tv2->vval.v_float; break;
			    case '-': f -= tv2->vval.v_float; break;
			    case '*': f *= tv2->vval.v_float; break;
			    case '/': f /= tv2->vval.v_float; break;
			}
			clear_tv(tv1);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f;
		    }
		    else
#endif
		    {
			switch (*op)
			{
			    case '+': n += tv_get_number(tv2); break;
			    case '-': n -= tv_get_number(tv2); break;
			    case '*': n *= tv_get_number(tv2); break;
			    case '/': n = num_divide(n, tv_get_number(tv2),
							       &failed); break;
			    case '%': n = num_modulus(n, tv_get_number(tv2),
							       &failed); break;
			}
			clear_tv(tv1);
			tv1->v_type = VAR_NUMBER;
			tv1->vval.v_number = n;
		    }
		}
		else
		{
		    if (tv2->v_type == VAR_FLOAT)
			break;

		    // str .= str
		    s = tv_get_string(tv1);
		    s = concat_str(s, tv_get_string_buf(tv2, numbuf));
		    clear_tv(tv1);
		    tv1->v_type = VAR_STRING;
		    tv1->vval.v_string = s;
		}
		return failed ? FAIL : OK;

	    case VAR_FLOAT:
#ifdef FEAT_FLOAT
		{
		    float_T f;

		    if (*op == '%' || *op == '.'
				   || (tv2->v_type != VAR_FLOAT
				    && tv2->v_type != VAR_NUMBER
				    && tv2->v_type != VAR_STRING))
			break;
		    if (tv2->v_type == VAR_FLOAT)
			f = tv2->vval.v_float;
		    else
			f = tv_get_number(tv2);
		    switch (*op)
		    {
			case '+': tv1->vval.v_float += f; break;
			case '-': tv1->vval.v_float -= f; break;
			case '*': tv1->vval.v_float *= f; break;
			case '/': tv1->vval.v_float /= f; break;
		    }
		}
#endif
		return OK;
	}
    }

    semsg(_(e_letwrong), op);
    return FAIL;
}